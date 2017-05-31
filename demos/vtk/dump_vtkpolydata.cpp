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

    using Point_type =
        ipme::data::Raster<ipme::data::Power<ipme::data::Point3d<double>>>;
    viz::Polydata<Point_type> data;

    for(unsigned int i = 0; i < 10000; ++i) {
        auto point = Point_type(xrand.next(), yrand.next(), zrand.next());
        ipme::data::set_raster(point, raster.next());
        ipme::data::set_power(point, power.next());

        data.add_point(point);
    }

    data.write_file("random_points.vtp");

    return EXIT_SUCCESS;
}
