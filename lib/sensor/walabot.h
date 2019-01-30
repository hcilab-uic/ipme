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

#ifndef CDI_SENSOR_WALABOT_H
#define CDI_SENSOR_WALABOT_H

#include <memory>
#include <string>

#include "receiver.h"

namespace ipme {
namespace sensor {
class Walabot {
public:
    struct Settings {
        struct Inner {
            Inner(double min_, double max_, double res_)
                : min{min_}, max{max_}, res{res_}
            {
            }
            double min;
            double max;
            double res;
        };

        //        class Radial : public Inner {
        //        };
        using Radial = Inner;
        using Theta = Inner;
        using Phi = Inner;

        Settings(const Radial& cm_, const Theta& degrees_, const Phi& phi_)
            : cm{cm_}, degrees{degrees_}, phi{phi_}
        {
        }

        Radial cm;
        Theta degrees;
        Phi phi;
    }; // struct Settings

    Walabot(const std::shared_ptr<Receiver> receiver, const Settings& settings,
            bool moving_target);
    virtual ~Walabot();
    void record_image_slice(const int iterations) const;
    void record_raw_image(const int iterations) const;
    void record_targets(const int iterations) const;

private:
    std::shared_ptr<Receiver> receiver_;
    const Settings settings_;
    const bool moving_target_;

    static const std::string settings_folder;
}; // class Walabot

} // namespace sensor
} // namespace ipme

#endif // CDI_SENSOR_WALABOT_H
