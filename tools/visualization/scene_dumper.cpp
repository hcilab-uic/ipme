#include <boost/program_options.hpp>
#include <memory>

#include "data/point3d.h"
#include "sensor/receiver3d.h"
#include "sensor/walabot3d.h"
#include "sensor/walabot_receiver_writer.h"
#include "utils/logger.h"

int main(int argc, char* argv[])
{
    cdi::utils::Logger::init(argv[0]);

    namespace po = boost::program_options;
    po::options_description desc("Options");

    // clang-format off
    desc.add_options()
            ("help", "Help message")
            ("filename,f",
             po::value<boost::filesystem::path>()->default_value("scene.vtp"),
             "output file name (.vtp)")
    ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    cdi::sensor::Settings::Radial radial{20, 500, 0.001};
    cdi::sensor::Settings::Theta theta{-15, 15, 0.00001};
    cdi::sensor::Settings::Phi phi{-90, 90, 0.0001};
    cdi::sensor::Settings settings{radial, theta, phi};

    using Point_t = cdi::data::Point3D<double>;

    INFO() << "Starting receiver";

    auto receiver =
        std::make_shared<cdi::sensor::Walabot_receiver_writer<Point_t>>();
    cdi::sensor::Walabot3d<Point_t> walabot{receiver, settings, false};
    walabot.record(1);

    INFO() << "Finished recording, writing to file";
    receiver->write(vm["filename"].as<boost::filesystem::path>());

    return EXIT_SUCCESS;
}
