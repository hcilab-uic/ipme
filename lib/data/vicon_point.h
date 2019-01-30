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
