#ifndef LIB_VISUALIZATION_POINT3D_H
#define LIB_VISUALIZATION_POINT3D_H

namespace cdi {
namespace visualization {
template <typename TCoordinate = double>
/**
 * @brief The Point3D class for storing 3-dimensional point
 */
struct Point3D {
    /**
     * @brief Point3D Constructor
     * @param x_ x-coordinate
     * @param y_ y-coordinate
     * @param z_ z-coordinate
     */
    Point3D(const TCoordinate x_, const TCoordinate y_, const TCoordinate z_);

    const TCoordinate x;
    const TCoordinate y;
    const TCoordinate z;
};

} // namespace visualization
} // namespace cdi

#include "point3d.ipp"

#endif // LIB_VISUALIZATION_POINT3D_H
