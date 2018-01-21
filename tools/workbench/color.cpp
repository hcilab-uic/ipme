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
