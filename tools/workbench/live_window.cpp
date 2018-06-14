#include "live_window.h"
#include "ui_live_window.h"

#include <QDebug>
#include <opencv2/core/mat.hpp>

Live_window::Live_window(QWidget* parent)
    : QDialog(parent), ui(new Ui::Live_window)
{
    ui->setupUi(this);

    capture_timer_ = new QTimer{this};
    connect(capture_timer_, &QTimer::timeout, this,
            &Live_window::process_video);
    capture_timer_->start(1000 / 24);
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
        cv::resize(frame, frame, cv::Size(640, 360), 0, 0, cv::INTER_CUBIC);
        QImage widget_image(static_cast<uchar*>(frame.data), frame.cols,
                            frame.rows, frame.step, QImage::Format_RGB888);

        ui->video_feed_label->setPixmap(QPixmap::fromImage(widget_image));
    }
}
