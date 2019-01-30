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

#ifndef CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H
#define CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H

#include <boost/filesystem.hpp>

#include "receiver3d.h"
#include "visualization/polydata.h"

namespace ipme {
namespace sensor {
template <typename Point_t>
class Walabot_receiver_writer : public Receiver3d<Point_t> {
public:
    using Data_t = visualization::Polydata<Point_t>;
    /**
     * @brief Walabot_receiver_writer constructor
     */
    Walabot_receiver_writer() : data_{std::make_unique<Data_t>()}
    {
    }

    /**
     * virtual destructor
     */
    virtual ~Walabot_receiver_writer() = default;

    /*! @copydoc Receiver3d::process(const Point_t&)
     */
    virtual void process(const Point_t& point);

    /**
     * @brief write function to write data to file
     * @param output_filename name of the file to write to
     */
    void write(const boost::filesystem::path& output_filename);

private:
    std::unique_ptr<Data_t> data_;
};

} // namespace sensor
} // namespace ipme

#include "walabot_receiver_writer.ipp"

#endif // CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H
