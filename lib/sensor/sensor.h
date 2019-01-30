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

#ifndef CDI_LIB_SENSOR_SENSOR_H
#define CDI_LIB_SENSOR_SENSOR_H

#include <string>
#include <vector>

namespace ipme {
namespace sensor {

using TargetId = int;

/**
 * @brief Structure representing a single reading of a sensor
 */
struct Reading {
    Reading(const double x_, const double y_, const double z_,
            const double amplitude_)
        : x{x_}, y{y_}, z{z_}, amplitude{amplitude_}
    {
    }

    double x;
    double y;
    double z;
    double amplitude;
};

/**
 * @brief The Spec_range struct to store settings ranges and resolution
 */
struct Spec_range {
    using Min = double;
    using Max = double;
    using Resolution = double;

    Spec_range(const Min min_, const Max max_, const Resolution resolution_);
    Spec_range(const std::vector<std::string>& range_vector);
    Spec_range(const std::string& range, const std::string& delim = ":");

    Min min;
    Max max;
    Resolution resolution;
};

/**
 * @brief The Settings struct to store sensor setting
 */
struct Settings {
    using Radial = Spec_range;
    using Theta = Spec_range;
    using Phi = Spec_range;

    Settings(const Radial& radial_cm_, const Theta& theta_degrees_,
             const Phi& phi_degrees_);

    Radial radial_cm;
    Theta theta_degrees;
    Phi phi_degrees;
};

/**
 * @brief The Sensor interface
 */
class Sensor {
public:
    /**
     * virtual destructor
     */
    virtual ~Sensor() = default;

    /**
     * @brief record a function that does that takes actual readings from the
     * sensor
     * @param iterations number of iterations to take. 0 means no limit.
     */
    virtual void record(const int iterations) = 0;
};

} // namespace sensor
} // namespace ipme
#endif // CDI_LIB_SENSOR_SENSOR_H
