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

#ifndef VISUALIZATION_WINDOW_H
#define VISUALIZATION_WINDOW_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <QMainWindow>
#include <QVector3D>
#include <Qt3DExtras/Qt3DWindow>

#include "config.h"
#include "frame.h"
#include "frame_collection.h"
#include "protobuf/scene.pb.h"
#include "scene_modifier.h"
#include "scene_visualization.h"
#include "similarity_finder.h"
#include "video_window.h"

namespace Ui {
class Visualization_window;
}

class Visualization_window
    : public QMainWindow,
      public ipme::wb::Scene_visualization,
      public std::enable_shared_from_this<Visualization_window> {
    Q_OBJECT

public:
    explicit Visualization_window(const ipme::wb::Config& config,
                                  QWidget* parent = 0);
    ~Visualization_window();

    void show_frame(int frame_index) override;

private slots:
    void on_action_show_top_view_triggered();

    void on_action_show_front_view_triggered();

    void on_action_previous_triggered();
    void on_action_next_triggered();
    void on_file_open_triggered();

    void on_action_launch_video_triggered();
    void display_frame_number(int frame_number);

    void on_save_outcome_button_clicked();

    void on_frame_policy_combobox_currentIndexChanged(const QString& arg1);

    void on_action_load_labels_triggered();

    void on_Visualization_window_destroyed();

    void on_replay_button_clicked();

    void on_findsimilar_button_clicked();

    void on_action_start_viz_triggered();

    void on_replay_section(size_t begin, size_t end);

    void on_find_similar(size_t begin, size_t end);

    void on_action_stop_viz_triggered();

    void on_action_pause_viz_triggered();

signals:
    void current_frame_number(int);

    void replay_section(size_t begin_frame, size_t end_frame);

    void find_similar(size_t begin_frame, size_t end_frame);

    void start_visualization();

private:
    void make_axes();
    void init();
    void show_current_frame();
    void make_axis(float x, float y, float z, float length,
                   const QColor& color);
    void make_axis(const QQuaternion& rotation, float length,
                   const QColor& color);
    void apply_frames_filter();

    ipme::wb::Frame filter(const ipme::wb::Frame& frame);

    int64_t frame_index_{0};
    Ui::Visualization_window* ui;
    Qt3DCore::QEntity* root_entity_ = nullptr;
    //    Qt3DExtras::Qt3DWindow* view_ = nullptr;
    std::shared_ptr<Qt3DExtras::Qt3DWindow> view_;
    std::unique_ptr<ipme::wb::Scene_modifier> scene_modifier_;
    //    std::vector<ipme::wb::Frame> frames_;
    std::shared_ptr<Video_window> video_window_;
    //    std::unordered_map<uint32_t, size_t> frame_index_map_;
    QString labeled_file_path_;
    ipme::wb::Frame_collection frames_;
    std::unordered_map<std::string, int> outcome_labels_;
    ipme::wb::Config config_;
    ipme::wb::Similarity_finder similarity_finder_{frames_};
};

#endif // VISUALIZATION_WINDOW_H
