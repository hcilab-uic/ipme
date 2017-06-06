#ifndef IPME_DATA_VICONPOINT_H
#define IPME_DATA_VICONPOINT_H

#include "point3d.h"

namespace ipme {
namespace data {
template <typename T>
struct Sourceid : T {
    unsigned int sourceid;
};

template <typename T>
struct Rotation : T {
    float rot_x = 0.f;
    float rot_y = 0.f;
    float rot_z = 0.f;
    float rot_w = 0.f;
};

using Viconpoint = Rotation<Sourceid<Point3d<float>>>;

} // namespace data
} // namespace ipme
#endif //  IPME_DATA_VICONPOINT_H
