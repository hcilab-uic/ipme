#include "live_window.h"
#include "ui_live_window.h"

#include <filesystem>
#include <mutex>
#include <shared_mutex>
#include <sstream>

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QScrollBar>
#include <QStatusBar>
#include <opencv2/core/mat.hpp>

#include "config.h"
#include "data/scene.h"
#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"
#include "sensor/vrpn_handler.h"
#include "utils/logger.h"
#include "utils/utils.h"

namespace {
QString get_default_rootdir()
{
    return QDir::homePath() + "/ipme_experiments";
}
} // namespace

Live_window::Live_window(const ipme::wb::Config& config, QWidget* parent)
    : QDialog{parent}, ui{new Ui::Live_window}, config_{config},
      output_root_dir_{get_default_rootdir()}, status_bar_{parent},
      omicron_thread_{nullptr}, scene_{std::make_shared<ipme::data::Scene>()},
      sage_handler_{scene_},
      vrpn_listener_{std::make_unique<ipme::sensor::Vrpn_handler>(scene_)}
{
    ui->setupUi(this);
    set_start_button_init();
    ui->video_device_index_edit->setText(
        QString::number(config_.video_device_index()));

    ui->vrpn_host_edit->setText(config_.vrpn_host().c_str());
    ui->vrpn_port_edit->setText(QString::number(config_.vrpn_port()));
    ui->vrpn_data_port_edit->setText(QString::number(config_.vrpn_data_port()));

    ui->sage_host_edit->setText(config_.sage_host().c_str());
    ui->sage_port_edit->setText(QString::number(config_.sage_port()));
    ui->sage_session_token_edit->setText(config_.sage_session_token().c_str());

    ui->screen_offset_x_edit->setText(QString::number(config_.offset_x()));
    ui->screen_offset_y_edit->setText(QString::number(config_.offset_y()));
    ui->screen_offset_z_edit->setText(QString::number(config_.offset_z()));

    ui->bottom_layout->addWidget(&status_bar_);
    set_status("Ready");
}

Live_window::~Live_window()
{
    set_state(ipme::wb::State_machine::State::uninitialized);
    if(ui) {
        delete ui;
        ui = nullptr;
    }
}

ipme::wb::State_machine::State Live_window::state() const
{
    std::shared_lock<std::shared_mutex> lock{state_mutex_};
    return state_;
}

void Live_window::set_state(const ipme::wb::State_machine::State state)
{
    std::unique_lock<std::shared_mutex> lock{state_mutex_};
    state_ = state;
}

void Live_window::on_start_experiment_button_clicked()
{
    if(is_uninitialized() || is_stopped()) {
        initialize_experiment();
    } else if(is_initialized() || is_paused() || is_stopped()) {
        start_experiment();
    } else if(is_running()) {
        // Enter pause
        stop_camera();
        set_start_button_start();
        set_state(ipme::wb::State_machine::State::paused);
        //        current_state_ = experiment_state::paused;
        show_message("Experiment paused");
    }
}

void Live_window::on_stop_experiment_button_clicked()
{
    stop_experiment();
}

void Live_window::process_video()
{
    if(!capture_.isOpened()) {
        return;
    }

    cv::Mat frame;
    cv::Mat display_frame;
    if(state() == ipme::wb::State_machine::State::initialized ||
       state() == ipme::wb::State_machine::State::running) {
        capture_ >> frame;

        if(frame.empty()) {
            TRACE() << "Empty frame";
            return;
        }

        cv::cvtColor(frame, display_frame, CV_BGR2RGB);
        //        cv::circle(frame, cv::Point{100, 100}, 50, cv::Scalar{255, 0,
        //        0});
        QImage widget_image(static_cast<uchar*>(display_frame.data),
                            display_frame.cols, display_frame.rows,
                            display_frame.step, QImage::Format_RGB888);
        ui->video_feed_label->setPixmap(QPixmap::fromImage(widget_image));

        // FIXME: There are two nested if blocks here. We should be able to do
        // this with one. Figure out how
        if(is_running()) {
            // If experiment is in running state, then record

            // FIXME: Also we are doing way too much work in the GUI thread
            // Please offload this to somewhere else
            ++frame_number_;

            update_frame_number(frame_number_);

            add_new_frame();

            ui->frame_num_value_label->setText(QString::number(frame_number_));

            auto ms = elapsed_time_.elapsed();
            auto seconds = ms / 1000;
            auto minutes = seconds / 60;
            auto hours = minutes / 60;

            auto display_time = QString{"%1:%2:%3"}
                                    .arg(hours, 2, 10, QLatin1Char{'0'})
                                    .arg(minutes % 60, 2, 10, QLatin1Char{'0'})
                                    .arg(seconds % 60, 2, 10, QLatin1Char{'0'});

            ui->elapsed_time_value_label->setText(display_time);

            video_writer_.write(frame);
        }
    }
}

void Live_window::process_vrpn()
{
    INFO() << "Starting VRPN poll";
    while(is_running()) {
        omicron_client_->poll();
    }

    INFO() << "Stopped VRPN";

    if(omicron_thread_) {
        DEBUG() << "Releasing omicron pointer";
        omicron_thread_.release();
    }
}

bool Live_window::initialize_vrpn()
{
    using omicron = omicronConnector::OmicronConnectorClient;
    omicron_client_ = std::make_unique<omicron>(&vrpn_listener_);

    const auto host = ui->vrpn_host_edit->text();
    const auto port = ui->vrpn_port_edit->text().toShort();
    const auto data_port = ui->vrpn_data_port_edit->text().toShort();
    std::stringstream ss;
    auto ret = omicron_client_->connect(host.toStdString().c_str(), port,
                                        data_port, 0, ss);

    std::string omicron_message{ss.str()};
    INFO() << omicron_message;
    show_message(omicron_message.c_str());

    set_status_indicator(ui->vrpn_status_indicator_label, ret);

    return ret;
}

bool Live_window::initialize_camera()
{
    return reset_camera();
}

void Live_window::init_file_setup()
{
    auto timestamp = ipme::utils::create_timestamp_string("%Y%m%d-%H%M%S");
    output_dir_ = output_root_dir_.toStdString() + "/" + timestamp;
    if(!std::filesystem::create_directories(output_dir_)) {
        throw std::runtime_error{"Could not create directory " +
                                 output_dir_.string()};
    }

    video_filename_ = output_dir_ / "video.avi";
    scene_->set_output_file_path(output_dir_ / "vrpn.pb");
}

bool Live_window::reset_camera()
{
    const int video_device_index = config_.video_device_index();
    capture_.open(video_device_index);

    const int frame_width =
        static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_WIDTH));
    const int frame_height =
        static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_HEIGHT));
    const int fps = static_cast<int>(capture_.get(CV_CAP_PROP_FPS));

    if(fps == 0) {
        ERROR() << "Error: could not get FPS information from camera";
        return false;
    }

    const auto codec = CV_FOURCC('M', 'J', 'P', 'G');
    const auto frame_size = cv::Size(frame_width, frame_height);

    auto frame_dimensions =
        QString{"Video dimensions: %1x%2"}.arg(frame_width).arg(frame_height);
    show_message(frame_dimensions);

    if(video_writer_.open(video_filename_, codec, fps, frame_size)) {
        INFO() << "Initialzed video file " << video_filename_;
    } else {
        std::stringstream ss;
        ss << "Couldn't open file " << video_filename_ << " to write";
        ERROR() << ss.str();
        throw std::runtime_error{ss.str()};
    }

    ui->frame_rate_value_label->setText(QString::number(fps));
    INFO() << "Streaming at " << fps << " frames per second";

    ui->video_feed_label->resize(frame_width, frame_height);
    int ms_per_frame = 1000 / fps;
    capture_timer_ = new QTimer{this};
    connect(capture_timer_, &QTimer::timeout, this,
            &Live_window::process_video);
    capture_timer_->start(ms_per_frame);

    set_status_indicator(ui->video_status_indicator_label, true);
    return true;
}

void Live_window::stop_camera()
{
    if(capture_timer_) {
        capture_timer_->stop();
        delete capture_timer_;
        capture_timer_ = nullptr;
    }

    capture_.release();
    video_writer_.release();

    DEBUG() << "Camera stopped";
    set_status("Camera stopped");

    set_status_indicator(ui->video_status_indicator_label, false);
}

void Live_window::shutdown()
{
    stop_experiment();
    set_state(ipme::wb::State_machine::State::uninitialized);
}

void Live_window::shutdown_vrpn()
{
    if(omicron_client_) {
        omicron_client_->dispose();
        omicron_client_.release();
    }

    set_status_indicator(ui->vrpn_status_indicator_label, false);
}

void Live_window::set_start_button_state(std::string_view text,
                                         std::string_view color)
{
    ui->start_experiment_button->setText(text.data());
    QString style{"font-weight: bold; font-size: 24px; background-color: " +
                  QString{color.data()} + ";"};
    //    show_message(style);
    ui->start_experiment_button->setStyleSheet(style);
}

void Live_window::set_status(std::string_view status, std::string_view color)
{
    QString style =
        "background-color: " + QString{color.data()} + ";" +
        "border-color: #000; border-style: inset; border: 2px solid #000";
    status_bar_.setStyleSheet(style);
    status_bar_.showMessage(status.data());
}

void Live_window::show_message(const QString& message)
{
    ui->log_window->insertPlainText(message + "\n");
    const auto scroll_bar = ui->log_window->verticalScrollBar();
    scroll_bar->setValue(scroll_bar->maximum());
}

void Live_window::enable_stop_button()
{
    ui->stop_experiment_button->setEnabled(true);
    ui->stop_experiment_button->setStyleSheet("background-color: red;"
                                              "font-size: 24px;"
                                              "font-weight: bold;");
    ui->stop_experiment_button->setText("Stop");
}

void Live_window::disable_stop_button()
{
    ui->stop_experiment_button->setText("");
    ui->stop_experiment_button->setStyleSheet("");
    ui->stop_experiment_button->setEnabled(false);
    ui->stop_experiment_button->setHidden(true);
}

void Live_window::update_frame_number(int frame_number)
{
    std::unique_lock<std::shared_mutex> lock{frame_number_mutex_};
    frame_number_ = frame_number;
}

int Live_window::frame_number() const
{
    std::shared_lock<std::shared_mutex> lock{frame_number_mutex_};
    return frame_number_;
}

void Live_window::add_new_frame()
{
    auto frame_index = frame_number();
    auto timestamp = frame_index * capture_timer_->interval();
    sage_handler_.flush();

    scene_->add_new_frame(frame_index, timestamp);
    DEBUG() << "Added new frame " << frame_index << " at time " << timestamp;
}

void Live_window::set_status_indicator(QWidget* widget, bool status)
{
    QString stylesheet =
        QString{"background: "} + (status ? "#48f442;" : "red;");
    stylesheet += "border-radius: 40px;"
                  "font-size: 22px;"
                  "font-weight: bold;"
                  "border-style: outset;";
    //    show_message("set to " + stylesheet);
    widget->setStyleSheet(stylesheet);
}

void Live_window::on_set_output_dir_button_clicked()
{
    output_root_dir_ = QFileDialog::getExistingDirectory(
        this, tr("Output Directory"), QDir::homePath());
    INFO() << "Output root directory changed to "
           << output_root_dir_.toStdString();
}

void Live_window::on_Live_window_destroyed()
{
    shutdown();
}

void Live_window::closeEvent(QCloseEvent* event)
{
    QDialog::closeEvent(event);
    shutdown();
}

void Live_window::initialize_experiment()
{
    init_file_setup();

    // Update config object from the UI entries because we expect the UI to
    // contain the most recent configuration

    // Screen offset
    double screen_offset_x = ui->screen_offset_x_edit->text().toDouble();
    double screen_offset_y = ui->screen_offset_y_edit->text().toDouble();
    double screen_offset_z = ui->screen_offset_z_edit->text().toDouble();

    config_.set_screen_offset(screen_offset_x, screen_offset_y,
                              screen_offset_z);

    // VRPN
    config_.set_vrpn_host(ui->vrpn_host_edit->text().toStdString());
    config_.set_vrpn_port(ui->vrpn_port_edit->text().toUShort());
    config_.set_vrpn_data_port(ui->vrpn_data_port_edit->text().toUShort());

    // SAGE2
    config_.set_sage_host(ui->sage_host_edit->text().toStdString());
    config_.set_sage_port(ui->sage_port_edit->text().toUShort());
    config_.set_sage_session_token(
        ui->sage_session_token_edit->text().toStdString());

    // Video
    config_.set_video_device_index(ui->video_device_index_edit->text().toInt());

    // initialization code
    auto camera_on = ui->on_camera_checkbox->isChecked();
    auto camera_status = camera_on ? initialize_camera() : !camera_on;
    if(!camera_status) {
        ERROR() << "Could not initialize camera";
        show_message("Could not initialize camera");
        return;
    }

    auto vrpn_on = ui->on_vrpn_checkbox->isChecked();
    auto vrpn_status = vrpn_on ? initialize_vrpn() : !vrpn_on;

    bool sage_status{false};
    try {
        auto sage_on = ui->on_sage_checkbox->isChecked();
        sage_status =
            sage_on ? sage_handler_.connect(config_.sage_host(),
                                            std::to_string(config_.sage_port()),
                                            config_.sage_session_token())
                    : !sage_on;
        set_status_indicator(ui->sage_status_indicator_label, sage_status);
        sage_handler_.set_state_machine(shared_from_this());
    } catch(const boost::system::system_error& err) {
        show_message(err.what());
        set_status("SAGE2 connection refused", "red");
    }

    if(camera_status && vrpn_status && sage_status) {
        set_start_button_start();
        set_state(ipme::wb::State_machine::State::initialized);
        show_message("Initialized");

        scene_->set_config(config_.scene_config());
    } else {
        WARN() << "Did not initialize correctly";

        if(camera_status) {
            stop_camera();
            WARN() << "Shutting down camera";
        } else {
            std::string msg{"Video stream initialization error"};
            show_message(msg.c_str());
            ERROR() << msg;
        }

        if(vrpn_status) {
            shutdown_vrpn();
            WARN() << "Shutting down VRPN";
        } else {
            std::string msg{"VRPN initialization error"};
            show_message(msg.c_str());
            ERROR() << msg;
        }

        if(sage_status) {
            sage_handler_.disconnect();
            set_status_indicator(ui->sage_status_indicator_label, false);
            WARN() << "Shutting down SAGE2";
        } else {
            std::string msg{"SAGE2 initialization error"};
            show_message(msg.c_str());
            ERROR() << msg;
        }
    }
}

void Live_window::start_experiment()
{
    // Start experiment
    set_state(ipme::wb::State_machine::State::running);
    if(!reset_camera()) {
        throw std::runtime_error{"Could not initialize camera"};
    }

    if(!sage_handler_.start()) {
        throw std::runtime_error{"Could not start SAGE2"};
    }

    if(!omicron_thread_) {
        DEBUG() << "Initializing omicron thread";
        omicron_thread_ =
            std::make_unique<std::thread>(&Live_window::process_vrpn, this);
    }

    set_start_button_pause();
    enable_stop_button();

    elapsed_time_.start();
    show_message("Experiment started");
}

void Live_window::stop_experiment()
{
    if(is_stopped() || is_uninitialized()) {
        INFO() << "Application in stopped state, ignore STOP message";
        return;
    }

    set_state(ipme::wb::State_machine::State::stopped);

    DEBUG() << "Joining omicron thread till it exits";

    if(omicron_thread_) {
        omicron_thread_->join();
    }

    shutdown_vrpn();

    sage_handler_.stop();

    stop_camera();
    scene_->reset();

    set_start_button_start();

    ui->video_feed_label->clear();
    ui->video_feed_label->close();
}
