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

#ifndef LIB_VISUALIZATION_POINT3D_H
#define LIB_VISUALIZATION_POINT3D_H

#include <limits>
#include <unordered_map>

// clang-format off
#define WITH_DOT(variable) variable.
#define ACCESSOR(variable, field) WITH_DOT(variable)##field

#define DECLARE_MIXIN(mixinname, fieldname, fieldtype, defaultvalue)           \
    template <typename T>                                                      \
    struct mixinname : public T {                                              \
        mixinname(const typename T::value_type x_,                             \
                  const typename T::value_type y_,                             \
                  const typename T::value_type z_) : T{x_, y_, z_} {}          \
                                                                               \
        fieldtype fieldname = defaultvalue;                                    \
    };                                                                         \
    template <typename T>                                                      \
    struct Has_ ## fieldname : std::false_type {};                             \
                                                                               \
    template <typename T>                                                      \
    struct Has_ ## fieldname<mixinname<T>> : std::true_type {};                \
                                                                               \
    template <typename T>                                                      \
    inline void set_ ## fieldname(T& point, const fieldtype fieldname)         \
    {                                                                          \
        if(Has_ ## fieldname<T>::value) {                                      \
            point.fieldname = fieldname;                                       \
        }                                                                      \
    }
// clang-format on

namespace ipme {
namespace data {
/**
 * @brief The Point3D class for storing 3-dimensional point
 */
template <typename TCoordinate = double>
struct Point3d {
    using value_type = TCoordinate;

    /**
     * @brief Point3D Constructor
     * @param x_ x-coordinate
     * @param y_ y-coordinate
     * @param z_ z-coordinate
     */
    Point3d(const TCoordinate x_, const TCoordinate y_, const TCoordinate z_)
        : x{x_}, y{y_}, z{z_}
    {
    }

    const TCoordinate x;
    const TCoordinate y;
    const TCoordinate z;
};

DECLARE_MIXIN(Raster, raster, int, 0)
DECLARE_MIXIN(Power, power, double, 0.0)
// DECLARE_MIXIN(Sourceid, sourceid, unsigned int,
//              std::numeric_limits<unsigned int>::max())

} // namespace data
} // namespace ipme

#include "point3d.ipp"

#endif // LIB_VISUALIZATION_POINT3D_H
