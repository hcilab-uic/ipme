#ifndef IPME_WB_POINT_H
#define IPME_WB_POINT_H

namespace ipme::wb {
struct Point {
    Point();

    Point(float x_, float y_, float z_);

    float distance(const Point& other) const;

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};
} // namespace ipme::wb

#endif // IPME_WB_POINT_H
