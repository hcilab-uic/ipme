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

#ifndef VIDEO_WINDOW_H
#define VIDEO_WINDOW_H

#include <memory>

#include <QMainWindow>
#include <QString>
#include <QTimer>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "scene_visualization.h"

namespace Ui {
class Video_window;
}

class Video_window : public QMainWindow {
    Q_OBJECT

public:
    explicit Video_window(QWidget* parent = 0);
    ~Video_window();

    void set_dirpath(const QString& dirpath);

    void set_scene_visualization(
        std::shared_ptr<ipme::wb::Scene_visualization> handle);

private slots:
    void on_action_play_triggered();

    void on_action_pause_triggered();

    void on_action_stop_triggered();

private:
    void load_video();
    void process_video();

    size_t frame_index_ = 0;
    bool play_video_ = false;
    Ui::Video_window* ui;
    QTimer* video_timer_{nullptr};
    QString dirpath_;
    int frame_width_ = 640;
    int frame_height_ = 360;
    cv::VideoCapture capture_;
    std::shared_ptr<ipme::wb::Scene_visualization> scene_handle_;
};

#endif // VIDEO_WINDOW_H
