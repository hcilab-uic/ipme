#include <iostream>

#include "connector/omicronConnectorClient.h"
#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"

int main()
{
    char buffer[16];
    memset(buffer, 0, sizeof(buffer));
    std::time_t time = std::time(nullptr);
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S",
                  std::localtime(&time));

    std::string filename{"vicon_record_" + std::string(buffer) + ".csv"};

    ipme::sensor::Vicon_listener listener{
        std::make_unique<ipme::sensor::Vicon_csvwriter>(filename)};
    omicronConnector::OmicronConnectorClient client(&listener);

    client.connect("131.193.77.108", 28000);

    while(true) {
        client.poll();
    }
}
