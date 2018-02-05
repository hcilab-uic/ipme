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

    show_intersections();

    glDisable(GL_STENCIL_TEST);

    glFlush();
}

void Scene_widget::clear_objects()
{
    objects_.clear();
    update();
}

void Scene_widget::set_ts_angle(float angle)
{
    for(auto& object : objects_) {
        object.set_ts_angle(angle);
    }
    repaint();
}

core::Point2f flatten_point(const core::Point3f& point)
{
    return core::Point2f{point.x(), point.y()};
}

core::Polygon2f flatten_polygon(const core::Polygon3f& polygon)
{
    std::vector<core::Point2f> points;
    for(const auto point : polygon.outer()) {
        points.push_back(flatten_point(point));
    }

    return core::Polygon2f{
        core::Polygon2f::ring_type{std::begin(points), std::end(points)}};
}

void Scene_widget::show_intersections() const
{
    if(!show_intersection_ || objects_.size() < 2) {
        return;
    }

    show_intersections(0, 6);
    show_intersections(0, 11);
    show_intersections(6, 11);

    show_intersections(1, 7);
    show_intersections(1, 12);
    show_intersections(7, 12);
}

void Scene_widget::show_intersections(size_t object_index1,
                                      size_t object_index2) const
{
    auto intersections = core::compute_intersection(
        flatten_polygon(objects_[object_index1].transaction_segment(60)),
        flatten_polygon(objects_[object_index2].transaction_segment(60)));

    qInfo() << "intersection count: " << intersections.size();

    for(const auto& polygon : intersections) {
        auto point2 = boost::geometry::return_centroid<core::Point2f>(polygon);
        core::Point3f point{point2.x(), point2.y(), 0.f};
        Geometry::draw_circle(point, .05f, Color{.3f, .3f, 0.f, .2f});
    }
}

} // namespace ipme::wb
