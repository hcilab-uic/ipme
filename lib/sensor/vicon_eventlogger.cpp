#include "vicon_eventlogger.h"

#include "utils/logger.h"

namespace ipme {
namespace sensor {
void Vicon_eventlogger::handle_trace(const oc::EventData& event)
{
    INFO() << "[TRACE] source id: " << event.sourceId;
}

void Vicon_eventlogger::handle_untrace(const oc::EventData& event)
{
    INFO() << "[UNTRACE] source id: " << event.sourceId;
}

void Vicon_eventlogger::handle_update(const oc::EventData& event)
{
    INFO() << "[UPDATE]";
    INFO() << "Motion capture source id: " << event.sourceId;
    if(oc::EventData::ServiceTypeMocap == event.serviceType) {
        INFO() << "event type: " << event.type
               << ", device: " << event.deviceTag;
    }

    INFO() << "[coord] X: " << event.posx << ", Y: " << event.posy
           << ", Z: " << event.posz;
    INFO() << "[rot  ] X: " << event.orx << ", Y: " << event.ory
           << ", Z: " << event.orz << ", W: " << event.orw;
    INFO() << "Flag: " << event.flags
           << ", extra data type: " << event.extraDataType << ", extra items "
           << event.extraDataItems << ", extra mask: " << event.extraDataMask;

    if(event.extraDataItems) {
        if(oc::EventData::ExtraDataFloatArray == event.extraDataType) {
            const float* ptr = reinterpret_cast<const float*>(event.extraData);
            for(unsigned int k = 0; k < event.extraDataType; ++k) {
                INFO() << "Value: " << k << ptr[k];
            }
        } else if(oc::EventData::ExtraDataVector3Array == event.extraDataType) {
            float value[3];
            for(int k = 0; k < event.extraDataItems; ++k) {
                event.getExtraDataVector3(k, value);
                INFO() << "Value: " << k << ": [" << value[0] << ", "
                       << value[1] << ", " << value[2];
            }
        }
    }
}

void Vicon_eventlogger::handle_up(const oc::EventData& event)
{
    INFO() << "[UP] source id: " << event.sourceId << ", type: " << event.type;
    INFO() << "Flag: " << event.flags
           << ", extra data type: " << event.extraDataType << ", extra items "
           << event.extraDataItems << ", extra mask: " << event.extraDataMask;
}

void Vicon_eventlogger::handle_down(const oc::EventData& event)
{
    INFO() << "[DOWN] source id: " << event.sourceId
           << ", type: " << event.type;
    INFO() << "Flag: " << event.flags
           << ", extra data type: " << event.extraDataType << ", extra items "
           << event.extraDataItems << ", extra mask: " << event.extraDataMask;
}

} // namespace sensor
} // namespace ipme
