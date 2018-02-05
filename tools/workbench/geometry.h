#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>

#include "color.h"
#include "core/point.h"

namespace ipme::wb {
#if defined(_WIN64) || defined(_WIN32)
static const float pi = static_cast<float>(std::acos(-1));
static const float two_pi = 2.f * pi;
static const float segment_angle = pi / 6.f;
static const float half_segment_angle = segment_angle / 2.f;
#else
static constexpr float pi = static_cast<float>(std::acos(-1));
static constexpr float two_pi = 2.f * pi;
static constexpr float segment_angle = pi / 6.f;
static constexpr float half_segment_angle = segment_angle / 2.f;
#endif

class Geometry {
public:
    static void draw_circle(const core::Point3f& point, float radius,
                            const Color& color = Color{});

    static void draw_square(const core::Point3f& point, float side,
                            const Color& color = Color{});

    static void draw_eq_triangle(const core::Point3f& point, float side,
                                 const Color& color = Color{});

    static void draw_line(const core::Point3f& point, float length, float angle,
                          const Color& color = Color{});

    static void draw_segment(const core::Point3f& point, float length,
                             float angle, const Color& color = Color{});
};

} // namespace ipme::wb

#endif // GEOMETRY_H
