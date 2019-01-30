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

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <cmath>

#include "color.h"
#include "core/point.h"
#include "core/polygon.h"

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

    static void draw_polygon(const core::Polygon3f& polygon,
                             const Color& color);

    static core::Polygon3f
    construct_circle_as_polygon(const core::Point3f& center, float radius);
};

} // namespace ipme::wb

#endif // GEOMETRY_H
