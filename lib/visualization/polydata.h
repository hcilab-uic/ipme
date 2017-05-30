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
