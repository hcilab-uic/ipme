#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "connector/omicronConnectorClient.h"
#include "io/json_parser.h"
#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"
#include "utils/logger.h"
#include "utils/omicron_config.h"
#include "utils/utils.h"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    ipme::utils::Logger::init(argv[0]);

    // clang-format off
    po::options_description config("Configuration");
    config.add_options()
            ("help", "Help Message")
            ("outputfile,o", po::value<std::string>(), "Output file")
            ("config,c",
             po::value<boost::filesystem::path>()->default_value("config.json"),
             "Path to config file")
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

        const auto configfile = vm["config"].as<boost::filesystem::path>();
        ipme::io::Json_parser parser{configfile};
        ipme::utils::Omicron_config config{parser};

        ipme::utils::Logger::set_severity(config.log_severity());

        INFO() << "Starting" << argv[0];
        INFO() << config.to_string();

        std::string filename;
        if(vm.count("outputfile")) {
            filename = vm["outputfile"].as<std::string>();
        } else {
            filename = "vicon_record_" +
                       ipme::utils::create_timestamp_string("%Y%m%d_%H%M%S") +
                       ".csv";
        }

        DEBUG() << "Output file name: " << filename;

        ipme::sensor::Vicon_listener listener{
            std::make_unique<ipme::sensor::Vicon_csvwriter>(
                filename, config.kinect_only_mode())};
        omicronConnector::OmicronConnectorClient client(&listener);

        client.connect(config.hostname().c_str(), config.port());
        while(!config.data_points() ||
              listener.event_count() < config.data_points()) {
            client.poll();
        }
    } catch(const std::exception& e) {
        FATAL() << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
