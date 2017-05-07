#ifndef LIB_VISUALIZATION_POINT3D_H
#define LIB_VISUALIZATION_POINT3D_H

#include <unordered_map>

namespace cdi {
namespace data {
/**
 * @brief The Point3D class for storing 3-dimensional point
 */
template <typename TCoordinate = double>
struct Point3D {
    /**
     * @brief Point3D Constructor
     * @param x_ x-coordinate
     * @param y_ y-coordinate
     * @param z_ z-coordinate
     */
    Point3D(const TCoordinate x_, const TCoordinate y_, const TCoordinate z_,
            const int raster_ = 0, const double power_ = 0.0);

    const TCoordinate x;
    const TCoordinate y;
    const TCoordinate z;

    /**
     * FIXME: This breaks the whole point of this concept being generic
     */
    const int raster;
    const double power;
};

} // namespace data
} // namespace cdi

#include "point3d.ipp"

#endif // LIB_VISUALIZATION_POINT3D_H
