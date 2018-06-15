#include "live_window.h"
#include "ui_live_window.h"

#include <mutex>
#include <shared_mutex>

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <opencv2/core/mat.hpp>

#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"
#include "utils/utils.h"

Live_window::Live_window(QWidget* parent)
    : QDialog(parent), ui(new Ui::Live_window), output_dir_{QDir::homePath()}
{
    ui->setupUi(this);

    capture_timer_ = new QTimer{this};
    connect(capture_timer_, &QTimer::timeout, this,
            &Live_window::process_video);
    capture_timer_->start(1000 / 24);

    // FIXME: This should come from elsewhere, not hard-coded here
    ui->vrpn_host_edit->setText("cave2tracker.evl.uic.edu");
    ui->vrpn_port_edit->setText("28000");
    initialize_vrpn();
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
    capture_.open(0);
    int frame_width = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_HEIGHT));
    ui->video_feed_label->resize(frame_width, frame_height);

    run_experiment_ = true;
}

void Live_window::on_stop_experiment_button_clicked()
{
    run_experiment_ = false;
    capture_.release();
    ui->video_feed_label->clear();
}

void Live_window::process_video()
{
    if(!capture_.isOpened()) {
        return;
    }

    cv::Mat frame;
    if(run_experiment_) {
        capture_ >> frame;

        if(frame.empty()) {
            qDebug() << "Empty frame";
            return;
        }

        cv::cvtColor(frame, frame, CV_BGR2RGB);
        //        cv::resize(frame, frame, cv::Size(640, 360), 0, 0,
        //        cv::INTER_CUBIC);
        QImage widget_image(static_cast<uchar*>(frame.data), frame.cols,
                            frame.rows, frame.step, QImage::Format_RGB888);

        int frame_num = static_cast<int>(capture_.get(CV_CAP_PROP_POS_FRAMES));
        update_frame_number(frame_num);
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

void Live_window::on_pushButton_clicked()
{
}

void Live_window::on_set_output_dir_button_clicked()
{
    output_dir_ = QFileDialog::getExistingDirectory(
        this, tr("Output Directory"), QDir::homePath());
}
