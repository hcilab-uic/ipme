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

    for(auto& object : objects_) {
        object.draw(show_centerline_, show_head_ts_, show_body_ts_,
                    show_device_ts_, ts_angles_);
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

void Scene_widget::set_head_ts_angle(float angle)
{
    ts_angles_.head = angle;
    repaint();
}

void Scene_widget::set_body_ts_angle(float angle)
{
    ts_angles_.body = angle;
    repaint();
}

void Scene_widget::set_device_ts_angle(float angle)
{
    ts_angles_.device = angle;
    repaint();
}

core::Point2f flatten_point(const core::Point3f& point)
{
    return core::Point2f{point.x(), point.y()};
}

core::Polygon2f flatten_polygon(const core::Polygon3f& polygon)
{
    std::vector<core::Point2f> points;
    for(const auto& point : polygon.outer()) {
        points.push_back(flatten_point(point));
    }

    return core::Polygon2f{
        core::Polygon2f::ring_type{std::begin(points), std::end(points)}};
}

core::Polygon3f unflatten_polygon(const core::Polygon2f& polygon)
{
    std::vector<core::Point3f> points;
    for(const auto& point : polygon.outer()) {
        points.push_back(core::Point3f{point.x(), point.y(), 0.f});
    }

    return core::Polygon3f{
        core::Polygon3f::ring_type{std::begin(points), std::end(points)}};
}

void Scene_widget::show_intersections() const
{
    if(objects_.size() < 2) {
        return;
    }

    static Color head_intersection_color{.5f, .2f, .2f, .15f};
    static Color body_intersection_color{.2f, .5f, .2f, .15f};
    static Color tight_coupling_color{.5f, .2f, .2f, .15f};
    static Color loose_coupling_color{.5f, .2f, .2f, .15f};

    if(show_head_intersection_) {
        show_intersections(0, 5, head_intersection_color);
        show_intersections(0, 10, head_intersection_color);
        show_intersections(5, 10, head_intersection_color);
    }

    if(show_body_intersection_) {
        show_intersections(1, 6, body_intersection_color);
        show_intersections(1, 11, body_intersection_color);
        show_intersections(6, 11, body_intersection_color);
    }

    if(show_tight_coupling_) {
        show_intersections(0, 4, tight_coupling_color);
        show_intersections(5, 9, tight_coupling_color);
        show_intersections(10, 14, tight_coupling_color);
    }

    if(show_loose_coupling_) {
        show_intersections(1, 4, loose_coupling_color);
        show_intersections(6, 9, loose_coupling_color);
        show_intersections(11, 14, loose_coupling_color);
    }
}

float compute_furthest_point_distance(const core::Polygon2f& polygon,
                                      const core::Point2f& point)
{
    float max_distance = 0.f;

    for(const auto& poly_point : polygon.outer()) {
        max_distance = std::max(
            static_cast<float>(boost::geometry::distance(point, poly_point)),
            max_distance);
    }

    return max_distance;
}

void Scene_widget::show_intersections(size_t object_index1,
                                      size_t object_index2,
                                      const Color& color) const
{
    auto p1 = flatten_polygon(objects_[object_index1].transaction_segment());
    auto p2 = flatten_polygon(objects_[object_index2].transaction_segment());
    auto intersections = core::compute_intersection(p1, p2);

    qInfo() << "intersection count =  " << intersections.size()
            << " for indices " << object_index1 << ", " << object_index2;

    auto circle_polygon =
        Geometry::construct_circle_as_polygon(core::Point3f{0, 0, 0}, 1.f);

    for(const auto& polygon : intersections) {
        auto clipped_polygons = core::compute_intersection(
            flatten_polygon(circle_polygon), polygon);

        for(const auto& clipped_polygon __attribute__((unused)) :
            // We are using the out-parameter version geometry::centroid()
            // instead of geometry::return_centroid() because of a GCC issue
            // that unnecessarily warns of an unused parameter in the function
            // return_centroid
            core::Point2f clipped_center{0.f, 0.f};
            boost::geometry::centroid<decltype(clipped_polygon), core::Point2f>(
                clipped_polygon, clipped_center);

            //            Geometry::draw_circle(
            //                core::Point3f{clipped_center.x(),
            //                clipped_center.y(), 0.f}, clipped_radius, color);
        }
    }
}

} // namespace ipme::wb
