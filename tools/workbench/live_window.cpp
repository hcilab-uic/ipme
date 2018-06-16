#include "live_window.h"
#include "ui_live_window.h"

#include <mutex>
#include <shared_mutex>

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QScrollBar>
#include <QStatusBar>
#include <opencv2/core/mat.hpp>

#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"
#include "utils/utils.h"

Live_window::Live_window(QWidget* parent)
    : QDialog{parent}, ui{new Ui::Live_window}, output_dir_{QDir::homePath()},
      status_bar_{parent}
{
    ui->setupUi(this);
    set_start_button_init();
    ui->video_device_index_edit->setText("0");

    // FIXME: This should come from elsewhere, not hard-coded here
    ui->vrpn_host_edit->setText("cave2tracker.evl.uic.edu");
    ui->vrpn_port_edit->setText("28000");
    initialize_vrpn();
    initialize_vrpn();
    initialize_sage();

    ui->sage_host_edit->setText("localhost");
    ui->sage_port_edit->setText("9292");

    ui->bottom_layout->addWidget(&status_bar_);
    set_status("Ready");
}

Live_window::~Live_window()
{
    if(capture_timer_) {
        delete capture_timer_;
        capture_timer_ = nullptr;
    }
    delete ui;
}

void Live_window::on_start_experiment_button_clicked()
{
    if(current_state_ == experiment_state::uninitialized) {
        // initialization code
        initialize_camera();
        set_start_button_start();
        current_state_ = experiment_state::initialized;

        show_message("Initialized");
    } else if(current_state_ == experiment_state::initialized ||
              current_state_ == experiment_state::paused ||
              current_state_ == experiment_state::stopped) {
        // Start experiment
        reset_camera();
        current_state_ = experiment_state::running;
        set_start_button_pause();
        enable_stop_button();

        show_message("Started");
    } else if(current_state_ == experiment_state::running) {
        // Enter pause
        stop_camera();
        set_start_button_start();
        current_state_ = experiment_state::paused;
        show_message("Paused");
    }
}

void Live_window::on_stop_experiment_button_clicked()
{
    stop_camera();
    ui->video_feed_label->clear();
    current_state_ = experiment_state::stopped;
    set_start_button_start();
}

void Live_window::process_video()
{
    if(!capture_.isOpened()) {
        return;
    }

    cv::Mat frame;
    if(current_state_ == experiment_state::running) {
        capture_ >> frame;

        if(frame.empty()) {
            qDebug() << "Empty frame";
            return;
        }

        ++frame_number_;
        update_frame_number(frame_number_);
        ui->frame_num_value_label->setText(QString::number(frame_number_));

        cv::cvtColor(frame, frame, CV_BGR2RGB);
        QImage widget_image(static_cast<uchar*>(frame.data), frame.cols,
                            frame.rows, frame.step, QImage::Format_RGB888);
        ui->video_feed_label->setPixmap(QPixmap::fromImage(widget_image));
    }
}

void Live_window::initialize_vrpn()
{
    std::string file_basename{
        "vrpn_record_" + ipme::utils::create_timestamp_string("%Y%m%d-%H%M%S") +
        ".csv"};
    std::string filepath{output_dir_.toStdString() + "/" + file_basename};
    ipme::sensor::Vicon_listener listener{
        std::make_unique<ipme::sensor::Vicon_csvwriter>(filepath, false)};
    omicron_client_ =
        std::make_unique<omicronConnector::OmicronConnectorClient>(&listener);

    const auto host = ui->vrpn_host_edit->text();
    const auto port = ui->vrpn_port_edit->text().toShort();
    omicron_client_->connect(host.toStdString().c_str(), port);
}

void Live_window::initialize_camera()
{
    capture_timer_ = new QTimer{this};
    connect(capture_timer_, &QTimer::timeout, this,
            &Live_window::process_video);
    reset_camera();
}

void Live_window::initialize_sage()
{
}

void Live_window::reset_camera()
{
    int video_device_index = ui->video_device_index_edit->text().toInt();
    capture_.open(video_device_index);
    int frame_width = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(capture_.get(CV_CAP_PROP_FPS));

    if(fps == 0) {
        qDebug() << "Error: could not get FPS information from camera";
        return;
    }

    ui->frame_rate_value_label->setText(QString::number(fps));
    qDebug() << "Streaming at " << fps << " frames per second";

    ui->video_feed_label->resize(frame_width, frame_height);
    int ms_per_frame = 1000 / fps;
    capture_timer_->start(ms_per_frame);
}

void Live_window::stop_camera()
{
    capture_.release();
    capture_timer_->stop();

    set_status("Camera stopped");
}

void Live_window::set_start_button_state(std::string_view text,
                                         std::string_view color)
{
    ui->start_experiment_button->setText(text.data());
    QString style{"font-weight: bold; font-size: 24px; background-color: " +
                  QString{color.data()} + ";"};
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

void Live_window::on_set_output_dir_button_clicked()
{
    output_dir_ = QFileDialog::getExistingDirectory(
        this, tr("Output Directory"), QDir::homePath());
}
