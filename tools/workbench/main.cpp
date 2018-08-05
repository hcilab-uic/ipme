#include "mainwindow.h"
#include <QApplication>

#include <boost/program_options.hpp>

#include "config.h"
#include "utils/logger.h"
#include "workbench_window.h"

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;

    po::options_description desc;

    std::string logfile_prefix{"log/" +
                               std::string{program_invocation_short_name}};
    std::filesystem::path config_path;
    // clang-format off
    desc.add_options()
        ("help", "help message")
        ("log-prefix,l", po::value(&logfile_prefix), "log file prefix")
        ("config,c", po::value(&config_path)->default_value("config.json"),
         "path to config file")
    ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("log-prefix")) {
        logfile_prefix = vm["log-prefix"].as<std::string>();
    }

    ipme::utils::Logger::init(logfile_prefix);
    ipme::utils::Logger::set_severity(boost::log::trivial::trace);

    if(vm.count("config")) {
        config_path = vm["config"].as<std::filesystem::path>();
    }

    ipme::wb::Config config{config_path};

    QApplication app{argc, argv};

    Workbench_window workbench{config};
    workbench.show();

    return app.exec();
}
