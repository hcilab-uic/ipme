#include "scene_widget.h"

#include <cmath>

#include <QDebug>

#include "color.h"
#include "core/point.h"
#include "geometry.h"

namespace ipme::wb {
Scene_widget::Scene_widget(QWidget* widget) : QOpenGLWidget(widget)
{
}

void Scene_widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_NEVER, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    glStencilMask(0xFF);
    glClear(GL_STENCIL_BUFFER_BIT);

    // boundary
    Geometry::draw_circle(core::Point3f{0.f, 0.f, 0.f}, 1.f,
                          Color{.1f, .1f, .1f, 1.f});

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilFunc(GL_EQUAL, 1, 0xFF);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Geometry::draw_circle(core::Point3f{0.f, 0.f, 0.f}, 1.f,
                          Color{.05f, .05f, .05f, 1.f});

    for(const auto& object : objects_) {
        object.draw(show_centerline_, show_tsegment_);
    }

    glDisable(GL_STENCIL_TEST);

    glFlush();
}

void Scene_widget::clear_objects()
{
    objects_.clear();
    update();
}

} // namespace ipme::wb
