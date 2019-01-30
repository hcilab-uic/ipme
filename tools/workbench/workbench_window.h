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

#ifndef WORKBENCH_WINDOW_H
#define WORKBENCH_WINDOW_H

#include <memory>

#include <QMainWindow>

#include "config.h"
#include "live_window.h"
#include "mainwindow.h"
#include "visualization_window.h"

namespace Ui {
class Workbench_window;
}

class Workbench_window : public QMainWindow {
    Q_OBJECT

public:
    explicit Workbench_window(const ipme::wb::Config& config, int screen_index,
                              QWidget* parent = 0);
    ~Workbench_window();

private slots:
    void on_button_launch_analysis_window_clicked();

    void on_button_launch_live_window_clicked();

private:
    Ui::Workbench_window* ui;
    std::shared_ptr<Live_window> live_window_;
    std::shared_ptr<MainWindow> main_window_;
    std::shared_ptr<Visualization_window> visualization_window_;
    cv::VideoCapture capture_;
};

#endif // WORKBENCH_WINDOW_H
