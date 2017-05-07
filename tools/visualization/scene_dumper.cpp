#include <boost/program_options.hpp>

#include "sensor/receiver3d.h"
#include "sensor/walabot3d.h"
#include "sensor/walabot_receiver_writer.h"

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description desc("Options");

    // clang-format off
    desc.add_options()
            ("help", "Help message")
    ;
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
}
