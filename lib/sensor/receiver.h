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

#ifndef CDI_SENSOR_RECEIVER_H
#define CDI_SENSOR_RECEIVER_H

#include "sensor.h"

namespace ipme {
namespace sensor {
class Receiver {
public:
    virtual ~Receiver() = default;
    virtual void process(const TargetId sensor_id, const Reading& reading) = 0;
    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) = 0;
};
} // namespace sensor
} // namespace ipme
#endif // CDI_SENSOR_RECEIVER_H
