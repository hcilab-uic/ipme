#include <boost/program_options.hpp>
#include <memory>

#include "data/point3d.h"
#include "sensor/receiver3d.h"
#include "sensor/walabot3d.h"
#include "sensor/walabot_receiver_writer.h"
#include "utils/logger.h"
#include "utils/string_utils.h"

int main(int argc, char* argv[])
{
    ipme::utils::Logger::init(argv[0]);

    namespace po = boost::program_options;
    po::options_description desc("Options");

    // clang-format off
    desc.add_options()
            ("help", "Help message")
            ("filename,f",
             po::value<boost::filesystem::path>()->default_value("scene.vtp"),
             "output file name (.vtp)")
            ("radial,r", po::value<std::string>()->default_value("0:500:0.001"),
             "Radian range and resolution")
            ("theta,t",
             po::value<std::string>()->default_value("-15:15:0.00001"),
             "Theta range and resolution")
            ("phi,p",
             po::value<std::string>()->default_value("-90:90:0.0001"),
             "Phi range and resolution")
    ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    ///// Setup R-Theta-Phi from command line arguments
    ipme::sensor::Settings::Radial radial{vm["radial"].as<std::string>(), ":"};
    ipme::sensor::Settings::Theta theta{vm["theta"].as<std::string>(), ":"};
    ipme::sensor::Settings::Phi phi{vm["phi"].as<std::string>(), ":"};
    ///////////////////////

    INFO() << "Settings";
    INFO() << "Radial: [" << radial.min << ", " << radial.max << ", "
           << radial.resolution << "]";
    INFO() << "Theta: [" << theta.min << ", " << theta.max << ", "
           << theta.resolution << "]";
    INFO() << "Phi: [" << phi.min << ", " << phi.max << ", " << phi.resolution
           << "]";

    ipme::sensor::Settings settings{radial, theta, phi};

    using Point_t =
        ipme::data::Raster<ipme::data::Power<ipme::data::Point3d<double>>>;

    INFO() << "Starting receiver";
    auto receiver =
        std::make_shared<ipme::sensor::Walabot_receiver_writer<Point_t>>();
    ipme::sensor::Walabot3d<Point_t> walabot{receiver, settings, false};
    walabot.record(1);

    INFO() << "Finished recording, writing to file";
    receiver->write(vm["filename"].as<boost::filesystem::path>());

    return EXIT_SUCCESS;
}
