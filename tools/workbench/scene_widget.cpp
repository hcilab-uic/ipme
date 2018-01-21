#include "scene_widget.h"

#include <cmath>

#include <QDebug>

#include "color.h"
#include "geometry.h"
#include "point.h"

namespace ipme::wb {
Scene_widget::Scene_widget(QWidget* widget) : QOpenGLWidget(widget)
{
}

void Scene_widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // boundary
    Geometry::draw_circle(Point{0.f, 0.f, 0.f}, 1.f, Color{.1f, .1f, .1f, 1.f});

    for(const auto& object : objects_) {
        object.draw(show_centerline_, show_tsegment_);
    }

    glFlush();
}

void Scene_widget::clear_objects()
{
    objects_.clear();
    update();
}

} // namespace ipme::wb
