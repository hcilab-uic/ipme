#include <random>

#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkVersion.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkXMLPolyDataWriter.h>

#include "data/point3d.h"
#include "utils/random.h"
#include "visualization/polydata.h"

int main()
{
    namespace viz = cdi::visualization;
    namespace util = cdi::utils;

    util::Random<double> xrand{0, 100};
    util::Random<double> yrand{0, 100};
    util::Random<double> zrand{0, 100};

    viz::Polydata<cdi::data::Point3D<double>> data;

    for(unsigned int i = 0; i < 10000; ++i) {
        data.add_point(cdi::data::Point3D<double>(xrand.next(), yrand.next(),
                                                  zrand.next()));
    }

    data.write_file("random_points.vtp");

    return EXIT_SUCCESS;
}
