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

#ifndef LIB_VISUALIZATION_POLYDATA_H
#define LIB_VISUALIZATION_POLYDATA_H

#include <boost/filesystem.hpp>
#include <memory>

#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include "data/point3d.h"

namespace ipme {
namespace visualization {
template <typename TPoint>
class Polydata {
public:
    /**
     * @brief Polydata Constructor
     */
    Polydata();

    /**
     * @brief add_point inline function to add point
     * @param point to be added
     */
    inline void add_point(const TPoint& point);

    /**
     * @brief write_file function to output file
     * @param output_filename file name to write to
     */
    void write_file(const boost::filesystem::path& output_filename);

private:
    size_t index_ = 0;

    vtkSmartPointer<vtkPoints> points_;
    vtkSmartPointer<vtkDoubleArray> point_powers_;
    vtkSmartPointer<vtkIntArray> point_rasters_;
    vtkSmartPointer<vtkIntArray> indices_;
};
} // namespace visualization
} // namespace ipme

#include "polydata.ipp"

#endif // LIB_VISUALIZATION_POLYDATA_H
