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

#include "vicon_csvwriter.h"

#include "utils/logger.h"

namespace ipme {
namespace sensor {
Vicon_csvwriter::Vicon_csvwriter(const std::string& outfilename,
                                 const bool kinect_only)
    : ofs_{outfilename, std::ios::out}, kinect_only_{kinect_only}
{
    if(kinect_only_) {
        ofs_ << "timestamp,frame_id,id,x,y,z" << std::endl;
    } else {
        ofs_ << "timestamp,id,device_tag,service_type,type,flags,x,y,z,rx,ry,"
                "rz,rw"
             << std::endl;
    }
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
    if(kinect_only_) {
        if(event.extraDataItems) {
            if(event.extraDataType == oc::EventData::ExtraDataFloatArray) {
                const float* ptr =
                    reinterpret_cast<const float*>(event.extraData);
                for(int k = 0; k < event.extraDataItems; ++k) {
                    INFO() << "val " << k << ": [" << ptr[k] << "]";
                }
            } else {
                float val[3];
                for(int k = 0; k < event.extraDataItems; ++k) {
                    event.getExtraDataVector3(k, val);
                    ofs_ << event.timestamp << "," << frame_id_ << "," << k
                         << "," << val[0] << "," << val[1] << "," << val[2]
                         << "\n";
                    INFO() << "val " << k << "[" << val[0] << "," << val[1]
                           << "," << val[2] << "]";
                }

                frame_id_++;
            }
        }
    } else {
        ofs_ << event.timestamp << "," << event.sourceId << ","
             << event.deviceTag << "," << event.serviceType << "," << event.type
             << "," << event.flags << "," << event.posx << "," << event.posy
             << "," << event.posz << "," << event.orx << "," << event.ory << ","
             << event.orz << "," << event.orw << "\n";
    }
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

} // namespace sensor
} // namespace ipme
