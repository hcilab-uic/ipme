#include <iostream>

#include "connector/omicronConnectorClient.h"
#include "sensor/vicon_csvwriter.h"
#include "sensor/vicon_listener.h"
#include "utils/utils.h"

int main()
{
    std::string filename{"vicon_record_" +
                         ipme::utils::create_timestamp_string("%Y%m%d_%H%M%S") +
                         ".csv"};

    ipme::sensor::Vicon_listener listener{
        std::make_unique<ipme::sensor::Vicon_csvwriter>(filename)};
    omicronConnector::OmicronConnectorClient client(&listener);

    client.connect("131.193.77.108", 28000);

    while(true) {
        client.poll();
    }
}
