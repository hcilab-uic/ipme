/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
    int screen_number = 0;
    // clang-format off
    desc.add_options()
        ("help", "help message")
        ("log-prefix,l", po::value(&logfile_prefix), "log file prefix")
        ("config,c", po::value(&config_path)->default_value("config.json"),
         "path to config file")
        ("screen,s", po::value(&screen_number)->default_value(0),
         "screen to launch the app on")
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

    Workbench_window workbench{config, screen_number};
    workbench.show();

    return app.exec();
}
