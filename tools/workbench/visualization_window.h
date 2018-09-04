#ifndef VISUALIZATION_WINDOW_H
#define VISUALIZATION_WINDOW_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <QMainWindow>
#include <QVector3D>
#include <Qt3DExtras/Qt3DWindow>

#include "frame.h"
#include "protobuf/scene.pb.h"
#include "scene_modifier.h"
#include "scene_visualization.h"
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
    explicit Visualization_window(QWidget* parent = 0);
    ~Visualization_window();

    void show_frame(int frame_index) override;

private slots:
    void on_action_show_top_view_triggered();

private slots:
    void on_action_show_front_view_triggered();

private slots:
    void on_action_previous_triggered();
    void on_action_next_triggered();
    void on_file_open_triggered();

    void on_action_launch_video_triggered();

private:
    void make_axes();
    void init();
    void show_current_frame();
    void make_axis(float x, float y, float z, float length,
                   const QColor& color);
    void make_axis(const QQuaternion& rotation, float length,
                   const QColor& color);

    int64_t frame_index_{0};
    Ui::Visualization_window* ui;
    Qt3DCore::QEntity* root_entity_ = nullptr;
    Qt3DExtras::Qt3DWindow* view_ = nullptr;
    std::unique_ptr<ipme::wb::Scene_modifier> scene_modifier_;
    std::vector<ipme::wb::Frame> frames_;
    std::unique_ptr<Video_window> video_window_;
    std::unordered_map<uint32_t, size_t> frame_index_map_;
};

#endif // VISUALIZATION_WINDOW_H
