/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "video_window.h"
#include "ui_video_window.h"

#include "utils/logger.h"
#include "visualization_window.h"

#include "opencv2/videoio.hpp"

Video_window::Video_window(QWidget* parent)
    : QMainWindow{parent}, ui{new Ui::Video_window}, video_timer_{
                                                         new QTimer{this}}
{
    ui->setupUi(this);

    connect(video_timer_, &QTimer::timeout, this, &Video_window::process_video);
    connect(this, &Video_window::play_video, this,
            &Video_window::on_action_play_triggered);
    connect(this, &Video_window::pause_video, this,
            &Video_window::on_action_pause_triggered);
    connect(this, &Video_window::stop_video, this,
            &Video_window::on_action_stop_triggered);
}

Video_window::~Video_window()
{
    mode_ = Mode::stop;

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

void Video_window::replay_section(size_t begin_frame, size_t end_frame)
{
    DEBUG() << "replaying section " << begin_frame << "-" << end_frame;
    replay_end_frame_ = end_frame + 1;
    mode_ = Mode::replay;
    play_video_ = false;
    frame_index_ = begin_frame;
    capture_.set(cv::CAP_PROP_POS_FRAMES, begin_frame);
}

void Video_window::on_action_play_triggered()
{
    mode_ = Mode::play;
}

void Video_window::on_set_video_frame(size_t frame_index)
{
    auto temp_mode = mode_;
    mode_ = Mode::pause;
    frame_index_ = frame_index;
    capture_.set(cv::CAP_PROP_POS_FRAMES, frame_index);
    mode_ = temp_mode;
}

void Video_window::on_action_pause_triggered()
{
    mode_ = Mode::pause;
}

void Video_window::on_action_stop_triggered()
{
    mode_ = Mode::stop;
}

void Video_window::load_video()
{
    auto video_path = dirpath_ + "/video.avi";
    capture_.open(video_path.toStdString());
    frame_width_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_WIDTH));
    frame_height_ = static_cast<int>(capture_.get(cv::CAP_PROP_FRAME_HEIGHT));
    ui->label_video->resize(frame_width_, frame_height_);

    //    double fps = capture_.get(cv::CAP_PROP_FPS);
    double fps = 24.0;
    INFO() << "FPS " << fps;

    int msec = static_cast<int>(1000.0 / fps);
    video_timer_->start(msec);

    INFO() << "setting timer delay = " << msec;
}

void Video_window::process_video()
{
    if(capture_.isOpened() &&
       ((mode_ == Mode::play) ||
        (mode_ == Mode::replay && frame_index_ <= replay_end_frame_))) {

        cv::Mat frame;
        capture_ >> frame;
        if(frame.empty()) {
            return;
        }

        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        cv::resize(frame, frame, cv::Size{frame_width_, frame_height_}, 0, 0,
                   cv::INTER_CUBIC);
        QImage widget_image(static_cast<uchar*>(frame.data), frame.cols,
                            frame.rows, frame.step, QImage::Format_RGB888);
        ui->label_video->setPixmap(QPixmap::fromImage(widget_image));

        scene_handle_->show_frame(frame_index_);
        ++frame_index_;
    }
}
