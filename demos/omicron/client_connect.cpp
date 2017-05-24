#include <iostream>

#include "connector/omicronConnectorClient.h"
#include "utils/logger.h"

using namespace omicronConnector;

class Connector_listener : public IOmicronConnectorClientListener {
  public:
    Connector_listener() {
        INFO() << "New listener";
    }

    virtual void onEvent(const EventData& event)
    {
        if(EventData::Trace == event.type) {
            INFO() << "Trace source: " << event.sourceId;;
        } else if(EventData::Untrace == event.type) {
            INFO() << "Untrace source: " << event.sourceId;
        } else if(EventData::Update == event.type) {
            if(EventData::Update == event.type) {
                INFO() << "MOCAP source " << event.sourceId << " type ";
                if(EventData::ServiceTypeMocap == event.serviceType) {
                    INFO() << event.type << " device " << event.deviceTag;
                }

                INFO() << "pos " << event.posx << "\t" << event.posy << "\t"
                       << event.posz;
                INFO() << "rot " << event.orx << "\t" << event.ory << "\t"
                       << event.orz << "\t" << event.orw;
                INFO() << "flag: " << event.flags;
                INFO() << "extra type: " << event.extraDataType;
                INFO() << "extra items: " << event.extraDataItems;
                INFO() << "extra mask: " << event.extraDataMask;

                if(event.extraDataItems) {
                    if(EventData::ExtraDataFloatArray == event.extraDataType) {
                        float* ptr = (float*)(event.extraData);
                        for(unsigned int k = 0; k < event.extraDataType; ++k) {
                            INFO() << "val " << k << ptr[k];
                        }
                    } else if(EventData::ExtraDataVector3Array ==
                              event.extraDataType) {
                        float val[3];
                        for(int k = 0; k < event.extraDataItems; ++k) {
                            event.getExtraDataVector3(k, val);
                            INFO() << "val " << k << ": [" << val[0] << ", "
                                   << val[1] << ", " << val[2];
                        }
                    }
                }
                // INFO() << "-------------" << std::endl;
            }
        } else if(EventData::Down == event.type) {

        } else if(EventData::Up == event.type) {
            INFO() << "EVENT up: " << event.sourceId << ", type: " << event.type
                   << std::endl;
            INFO() << "flag: " << event.flags << std::endl;
            INFO() << "extra: " << event.extraDataType << std::endl;
            INFO() << "items: " << event.extraDataItems << std::endl;
            INFO() << "mask: " << event.extraDataMask << std::endl;
            INFO() << "---------------" << std::endl;
        }
    }
};

int main(int /* argc */, char* argv[])
{
    cdi::utils::Logger::init(argv[0]);

    INFO() << "Starting listener";

    Connector_listener listener;
    OmicronConnectorClient client(&listener);
    client.connect("131.193.77.108", 28000);
    while(true) {
        client.poll();
    }
}
