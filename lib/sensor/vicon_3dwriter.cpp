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

#include "vicon_3dwriter.h"

namespace ipme {
namespace sensor {
Vicon_3dwriter::~Vicon_3dwriter()
{
    polydata_writer_.write_file(outfile_name_);
}

void Vicon_3dwriter::handle_trace(const oc::EventData& /* event */)
{
}

void Vicon_3dwriter::handle_untrace(const oc::EventData& /* event */)
{
}

void Vicon_3dwriter::handle_update(const oc::EventData& event)
{
    polydata_writer_.add_point(data::Point3d<double>{
        static_cast<double>(event.posx), static_cast<double>(event.posy),
        static_cast<double>(event.posz)});
}

void Vicon_3dwriter::handle_up(const oc::EventData& /* event */)
{
}

void Vicon_3dwriter::handle_down(const oc::EventData& /* event */)
{
}
} // namespace sensor
} // namespace ipme
