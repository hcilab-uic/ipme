#include "point.h"

#include <cmath>

namespace ipme::wb {

Point::Point() = default;

Point::Point(float x_, float y_, float z_) : x{x_}, y{y_}, z{z_}
{
}

float Point::distance(const Point& other) const
{
    return static_cast<float>(std::sqrt(std::pow(x - other.x, 2) +
                                        std::pow(y - other.y, 2) +
                                        std::pow(z - other.z, 2)));
}

} // namespace ipme::wb
