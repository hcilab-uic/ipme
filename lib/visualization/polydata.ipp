#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include <iostream>

namespace cdi {
namespace visualization {
template <typename TPoint>
Polydata<TPoint>::Polydata()
    : points_{vtkSmartPointer<vtkPoints>::New()},
      point_powers_{vtkSmartPointer<vtkDoubleArray>::New()},
      point_rasters_{vtkSmartPointer<vtkIntArray>::New()}
{
    point_powers_->SetNumberOfComponents(1);
    point_powers_->SetName("Power");

    point_rasters_->SetNumberOfComponents(1);
    point_rasters_->SetName("Raster");
}

template <typename TPoint>
void Polydata<TPoint>::write_file(
    const boost::filesystem::path& output_filename)
{
    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points_);

    auto filter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    filter->AddInputData(polydata);
    filter->Update();
    polydata = filter->GetOutput();

    polydata->GetPointData()->AddArray(point_powers_);
    polydata->GetPointData()->AddArray(point_rasters_);

    auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetFileName(output_filename.string().c_str());
    writer->SetInputData(polydata);

    writer->Write();
}

template <typename TPoint>
void Polydata<TPoint>::add_point(const TPoint& point)
{
    points_->InsertNextPoint(point.x, point.y, point.z);
    point_rasters_->InsertNextValue(point.raster);
    point_powers_->InsertNextValue(point.power);
}

} // namespace visualizaton
} // namespace cdi
