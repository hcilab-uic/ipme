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

#include "color.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ipme::wb {
Color::Color() = default;

Color::Color(float r_, float g_, float b_, float a_)
    : r{r_}, g{g_}, b{b_}, a{a_}
{
}

// Color::Color(const Color& other)
//    : r{other.r}, g{other.g}, b{other.b}, a{other.a}
//{
//}

// Color& Color::operator=(const Color& other)
//{
//    r = other.r;
//    g = other.g;
//    b = other.b;
//    a = other.a;
//}

QString Color::hex_string() const
{
    std::stringstream ss;

    char s[8];
    std::fill(std::begin(s), std::end(s), 0);

    snprintf(s, sizeof(s), "#%02x%02x%02x",
             static_cast<unsigned int>(255.f * r),
             static_cast<unsigned int>(255.f * g),
             static_cast<unsigned int>(255.f * b));
    return QString{s};
}

QString Color::stylesheet(const QString& fieldname) const
{
    return QString{fieldname + ": " + hex_string()};
}

} // namespace ipme::wb
