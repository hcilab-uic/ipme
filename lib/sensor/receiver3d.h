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

#ifndef CDI_LIB_SENSOR_RECEIVER3D_H
#define CDI_LIB_SENSOR_RECEIVER3D_H

namespace ipme {
namespace sensor {
template <typename TPoint>
class Receiver3d {
public:
    virtual ~Receiver3d() = default;

    /**
     * @brief process the point information that is passed by the sensor
     * @param point 3d point with or without other attributes
     */
    virtual void process(const TPoint& point) = 0;
};
} // namespace sensor
} // namespace ipme
#endif // CDI_LIB_SENSOR_RECEIVER3D_H
