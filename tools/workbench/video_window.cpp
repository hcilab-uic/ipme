#include "video_window.h"
#include "ui_video_window.h"

#include "utils/logger.h"

Video_window::Video_window(QWidget* parent)
    : QMainWindow{parent}, ui{new Ui::Video_window}, video_timer_{
                                                         new QTimer{this}}
{
    ui->setupUi(this);

    connect(video_timer_, &QTimer::timeout, this, &Video_window::process_video);
}

Video_window::~Video_window()
{
    delete ui;
}

void Video_window::set_dirpath(const QString& dirpath)
{
    dirpath_ = dirpath;
    load_video();
}

void Video_window::set_scene_visualization(
    std::shared_ptr<ipme::wb::Scene_visualization> handle)
{
    scene_handle_ = handle;
}

void Video_window::set_fps(int fps)
{
    if(video_timer_->isActive()) {
        video_timer_->stop();
    }

    int msec = static_cast<int>(1000.f / fps);
    video_timer_->start(msec);
    INFO() << "setting timer delay = " << msec;
}

void Video_window::set_speed_percent(int percent)
{
    set_fps(original_fps_ * percent * .01);
}

void Video_window::on_action_play_triggered()
{
    play_video();
}

void Video_window::on_action_pause_triggered()
{
    pause_video();
}

void Video_window::on_action_stop_triggered()
{
    stop_video();
}

void Video_window::load_video()
{
    auto video_path = dirpath_ + "/video.avi";
    capture_.open(video_path.toStdString());
    frame_width_ = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_WIDTH));
    frame_height_ = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_HEIGHT));
    frame_count_ = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_COUNT));
    ui->label_video->resize(frame_width_, frame_height_);

    original_fps_ = capture_.get(CV_CAP_PROP_FPS);
    set_fps(original_fps_);

    INFO() << "Original FPS " << original_fps_;
}

void Video_window::process_video()
{
    if(capture_.isOpened() && play_video_) {
        cv::Mat frame;
        capture_ >> frame;
        if(frame.empty()) {
            return;
        }

        cv::cvtColor(frame, frame, CV_BGR2RGB);
        cv::resize(frame, frame, cv::Size{frame_width_, frame_height_}, 0, 0,
                   cv::INTER_CUBIC);
        QImage widget_image(static_cast<uchar*>(frame.data), frame.cols,
                            frame.rows, frame.step, QImage::Format_RGB888);
        ui->label_video->setPixmap(QPixmap::fromImage(widget_image));

        scene_handle_->show_frame(frame_index_);
        ++frame_index_;
    }
}
