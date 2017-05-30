#include "vicon_3dwriter.h"

namespace ipme {
namespace sensor {
Vicon_3dwriter::~Vicon_3dwriter()
{
    polydata_writer_.write_file(outfile_name_);
}

// void Vicon_3dwriter::write(const boost::filesystem::path& outfile_name)
//{
//    polydata_writer_.write_file(outfile_name);
//}

void Vicon_3dwriter::handle_trace(const oc::EventData& /* event */)
{
}

void Vicon_3dwriter::handle_untrace(const oc::EventData& /* event */)
{
}

void Vicon_3dwriter::handle_update(const oc::EventData& event)
{
    polydata_writer_.add_point(data::Point3D<double>{
        static_cast<double>(event.posx), static_cast<double>(event.posy),
        static_cast<double>(event.posz)});
}

void Vicon_3dwriter::handle_up(const oc::EventData& /* event */)
{
}

void Vicon_3dwriter::handle_down(const oc::EventData& /* event */)
{
}
} // namespace sensor
} // namespace ipme
