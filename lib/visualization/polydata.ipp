#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

namespace cdi {
namespace visualization {
template <typename TPoint>
Polydata<TPoint>::Polydata() : points_{vtkSmartPointer<vtkPoints>::New()}
{
}

template<typename TPoint>
void Polydata<TPoint>::write_file(
    const boost::filesystem::path& output_filename)
{
    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points_);

    auto filter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    filter->AddInputData(polydata);
    filter->Update();
    polydata = filter->GetOutput();

    auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetFileName(output_filename.string().c_str());
    writer->SetInputData(polydata);

    writer->Write();
}
} // namespace visualizaton
} // namespace cdi
