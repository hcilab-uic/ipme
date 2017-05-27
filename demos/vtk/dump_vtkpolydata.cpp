#include <random>

#include "data/point3d.h"
#include "utils/random.h"
#include "visualization/polydata.h"

int main()
{
    namespace viz = ipme::visualization;
    namespace util = ipme::utils;

    util::Random<double> xrand{0, 100};
    util::Random<double> yrand{0, 100};
    util::Random<double> zrand{0, 100};
    util::Random<int> raster{0, 255};
    util::Random<double> power{0, 50};

    viz::Polydata<ipme::data::Point3D<double>> data;

    for(unsigned int i = 0; i < 10000; ++i) {
        data.add_point(ipme::data::Point3D<double>(xrand.next(), yrand.next(),
                                                  zrand.next(), raster.next(),
                                                  power.next()));
    }

    data.write_file("random_points.vtp");

    return EXIT_SUCCESS;
}
