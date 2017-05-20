#include <iostream>

#include "connector/omicronConnectorClient.h"

using namespace omicronConnector;

class Connector_listener : public IOmicronConnectorClientListener {
  public:
    virtual void onEvent(const EventData& event)
    {
        if(EventData::Trace == event.type) {
            std::cerr << "Trace source: " << event.sourceId << std::endl;
        } else if(EventData::Untrace == event.type) {
            std::cerr << "Untrade sound: " << event.sourceId << std::endl;
        } else if(EventData::Update == event.type) {
            if(EventData::Update == event.type) {
                if(EventData::ServiceTypeMocap == event.serviceType) {
                    std::cerr << "MOCAP source " << event.sourceId << " type "
                              << event.type << " device " << event.deviceTag
                              << std::endl;
                }

                std::cout << "pos " << event.posx << "\t" << event.posy << "\t"
                          << event.posz << std::endl;
                std::cout << "rot " << event.orx << "\t" << event.ory << "\t"
                          << event.orz << "\t" << event.orw << std::endl;
                std::cout << "flag: " << event.flags << std::endl;
                std::cout << "extra type: " << event.extraDataType << std::endl;
                std::cout << "extra items: " << event.extraDataItems
                          << std::endl;
                std::cout << "extra mask" << event.extraDataMask << std::endl;

                if(event.extraDataItems) {
                    if(EventData::ExtraDataFloatArray == event.extraDataType) {
                        float* ptr = (float*)(event.extraData);
                        for(unsigned int k = 0; k < event.extraDataType; ++k) {
                            std::cout << "val " << k << ptr[k] << std::endl;
                        }
                    } else if(EventData::ExtraDataVector3Array ==
                              event.extraDataType) {
                        float val[3];
                        for(int k = 0; k < event.extraDataItems; ++k) {
                            event.getExtraDataVector3(k, val);
                            std::cout << "val " << k << ": [" << val[0] << ", "
                                      << val[1] << ", " << val[2] << std::endl;
                        }
                    }
                }
                std::cout << "-------------" << std::endl;
            }
        } else if(EventData::Down == event.type) {

        } else if(EventData::Up == event.type) {
            std::cout << "EVENT up: " << event.sourceId
                      << ", type: " << event.type << std::endl;
            std::cout << "flag: " << event.flags << std::endl;
            std::cout << "extra: " << event.extraDataType << std::endl;
            std::cout << "items: " << event.extraDataItems << std::endl;
            std::cout << "mask: " << event.extraDataMask << std::endl;
            std::cout << "---------------" << std::endl;
        }
    }
};

int main()
{
    Connector_listener listener;
    OmicronConnectorClient client(&listener);
    client.connect("131.193.77.108", 28000);
    while(true) {
        client.poll();
    }
}
