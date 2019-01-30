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

#include <memory>

namespace ipme {
namespace sensor {
// template <typename Point_t>
// Walabot_receiver_writer<Point_t>::Walabot_receiver_writer()
//    : data_{std::make_unique<visualization::Polydata<Point_t>>()}
//{
//}

template <typename Point_t>
void Walabot_receiver_writer<Point_t>::process(const Point_t& point)
{
    data_->add_point(point);
}

template <typename Point_t>
void Walabot_receiver_writer<Point_t>::write(
    const boost::filesystem::path& output_filename)
{
    data_->write_file(output_filename);
}

} // namespace sensor
} // namespace ipme
