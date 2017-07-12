#include "vicon_csvwriter.h"

#include "utils/logger.h"

namespace ipme {
namespace sensor {
Vicon_csvwriter::Vicon_csvwriter(const std::string& outfilename)
    : ofs_{outfilename, std::ios::out}
{
    ofs_ << "timestamp,id,device_tag,service_type,type,flags,x,y,z,rx,ry,rz,rw"
         << std::endl;
}

Vicon_csvwriter::~Vicon_csvwriter()
{
    ofs_.close();
}

void Vicon_csvwriter::handle_trace(const oc::EventData& /* event */)
{
}

void Vicon_csvwriter::handle_untrace(const oc::EventData& /* event */)
{
}

void Vicon_csvwriter::handle_update(const omicronConnector::EventData& event)
{
    ofs_ << event.timestamp << "," << event.sourceId << "," << event.deviceTag
         << "," << event.serviceType << "," << event.type << "," << event.flags
         << "," << event.posx << "," << event.posy << "," << event.posz << ","
         << event.orx << "," << event.ory << "," << event.orz << ","
         << event.orw << "\n";

    const auto serviceid =
        event.deviceTag & omicron::EventBase::DeviceTagMask::DTServiceIdMask;
    INFO() << "user id:" << serviceid;
}

void Vicon_csvwriter::handle_up(const oc::EventData& /* event */)
{
}

void Vicon_csvwriter::handle_down(const oc::EventData& /* event */)
{
}

} // sensor
} // ipme
