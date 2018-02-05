#include "geometry.h"

#include <GL/glut.h>

namespace ipme::wb {
void Geometry::draw_circle(const core::Point3f& point, float radius,
                           const Color& color)
{
    static constexpr int iteration_count = 40;

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(color.r, color.g, color.b);
    glVertex3f(point.x(), point.y(), point.z());

    for(int i = 0; i <= iteration_count; ++i) {
        const float x = radius * std::cos(i * two_pi / iteration_count);
        const float y = radius * std::sin(i * two_pi / iteration_count);
        glVertex3f(point.x() + x, point.y() + y, 0.f);
    }
    glEnd();
}

void Geometry::draw_square(const core::Point3f& point, float side,
                           const Color& color)
{
    const float offset = side * .5f;

    glColor3f(color.r, color.g, color.b);
    glBegin(GL_POLYGON);
    glVertex3f(point.x() + offset, point.y() + offset, point.z());
    glVertex3f(point.x() + offset, point.y() - offset, point.z());
    glVertex3f(point.x() - offset, point.y() - offset, point.z());
    glVertex3f(point.x() - offset, point.y() + offset, point.z());
    glEnd();
}

void Geometry::draw_eq_triangle(const core::Point3f& /* point */,
                                float /* side */, const Color& /* color */)
{
}

void Geometry::draw_line(const core::Point3f& point, float length, float angle,
                         const Color& color)
{
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);

    glVertex3f(point.x(), point.y(), point.z());
    glVertex3f(point.x() + (length * std::cos(angle)),
               point.y() + (length * std::sin(angle)), point.z());

    glEnd();
}

void Geometry::draw_segment(const core::Point3f& point, float length,
                            float angle, const Color& color)
{
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, .1f);

    glVertex3f(point.x(), point.y(), point.z());

    const auto left_angle = angle - half_segment_angle;
    glVertex3f(point.x() + (length * std::cos(left_angle)),
               point.y() + (length * std::sin(left_angle)), point.z());

    const auto right_angle = angle + half_segment_angle;
    glVertex3f(point.x() + (length * std::cos(right_angle)),
               point.y() + (length * std::sin(right_angle)), point.z());

    glEnd();
}

void Geometry::draw_polygon(const core::Polygon3f& polygon, const Color& color)
{
    glBegin(GL_TRIANGLES);
    glColor4f(color.r, color.g, color.b, .1f);

    for(const auto& point : polygon.outer()) {
        glVertex3f(point.x(), point.y(), point.z());
    }

    glEnd();
}

} // namespace ipme::wb
