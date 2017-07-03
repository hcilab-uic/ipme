#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "connector/omicronConnectorClient.h"
#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"
#include "utils/utils.h"
#include "utils/logger.h"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ipme::utils::Logger::init(argv[0]);

    // clang-format off
    po::options_description config("Configuration");
    config.add_options()
        ("help", "Help Message")
        ("host,h", po::value<std::string>()->default_value("131.193.77.108"),
        "Host Address")
        ("port,p", po::value<short>()->default_value(28000), "Port")
        ("outputfile,o", po::value<std::string>(), "Output file")
        ("numpoints,n", po::value<unsigned int>()->required(),
         "Number of points to collect")
    ;
    // clang-format on

    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, config), vm);
        po::notify(vm);

        if(vm.count("help")) {
            std::cout << config << "\n";
            return 0;
        }

        std::string host = vm["host"].as<std::string>();
        const short port = vm["port"].as<short>();
        const unsigned int num_points = vm["numpoints"].as<unsigned int>();

        INFO() << "Connecting to: " << host << ":" << port;

        std::string filename;
        if(vm.count("outputfile")) {
            filename = vm["outputfile"].as<std::string>();
        } else {
            filename = "vicon_record_" +
                       ipme::utils::create_timestamp_string("%Y%m%d_%H%M%S") +
                       ".csv";
        }

        DEBUG() << "Output file name: " << filename;
        DEBUG() << "Number of points: " << num_points;

        ipme::sensor::Vicon_listener listener{
            std::make_unique<ipme::sensor::Vicon_csvwriter>(filename)};
        omicronConnector::OmicronConnectorClient client(&listener);

        client.connect(host.c_str(), port);
        while(listener.event_count() < num_points) {
            client.poll();
        }
    } catch(const std::exception& e) {
        FATAL() << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
