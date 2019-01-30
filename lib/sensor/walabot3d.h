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

#ifndef CDI_LIB_SENSOR_WALABOT3D_H
#define CDI_LIB_SENSOR_WALABOT3D_H

#include <boost/filesystem.hpp>
#include <memory>

#include "receiver3d.h"
#include "sensor.h"

namespace ipme {
namespace sensor {
template <typename Point_t>
class Walabot3d : public Sensor {
public:
    /**
     * @brief Walabot3d Constructor
     * @param receiver polymorphic handle
     * @param settings sensor settings
     */
    Walabot3d(const std::shared_ptr<Receiver3d<Point_t>>& receiver,
              const Settings& settings, bool moving_target);

    /*! @copydoc Sensor::~Sensor()
     */
    virtual ~Walabot3d();

    /*! @copydoc Sensor::record(const int)
     */
    virtual void record(const int iterations) override;

private:
    void process_snapshot(int* raster_image, int size_x, int size_y, int size_z,
                          double power);

    std::shared_ptr<Receiver3d<Point_t>> receiver_;
    const Settings settings_;
    const Spec_range::Min theta_span_;
    const Spec_range::Min phi_span_;
    const Spec_range::Min r_span_;

    static const boost::filesystem::path settings_folder;
};
} // namespace sensor
} // namespace ipme

#include "walabot3d.ipp"
#endif // CDI_LIB_SENSOR_WALABOT3D_H
