#include "frame.h"

#include <QQuaternion>

#include "utils/logger.h"

namespace ipme::wb {
/* static */ Frame Frame::create_from_pb(
    const ipme::scene::Frame& scene_frame, const ipme::scene::Position& offset,
    const std::unordered_map<uint32_t, std::string>& registered_objects)
{
    Frame frame;
    size_t device_count{0};
    size_t people_count{0};
    for(const auto& vrpn_object : scene_frame.vrpn_objects()) {
        const auto source_id = vrpn_object.vrpn_source_id();
        const auto itr = registered_objects.find(source_id);

        if(itr == std::end(registered_objects)) {
            continue;
        }

        const auto type = itr->second;
        if(type.find("person") != std::string::npos) {
            frame.persons.push_back(vrpn_object.pose());
            ++device_count;
        } else if(type.find("computer") != std::string::npos) {
            frame.devices.push_back(vrpn_object.pose());
            ++people_count;
        } else {
            ERROR() << "Unknown type " << type;
        }
    }

    for(const auto& screen_object : scene_frame.screen_objects()) {
        frame.screen_objects.emplace_back(
            Screen_object{screen_object.position(), offset,
                          static_cast<float>(screen_object.size().width()),
                          static_cast<float>(screen_object.size().height())});
    }

    INFO() << "Loaded " << people_count << " people and " << device_count
           << " devices " << scene_frame.screen_objects().size()
           << " screen objects";

    return frame;
}

Screen_object::Screen_object(const ipme::scene::Position& position,
                             const ipme::scene::Position& offset, float width_,
                             float height_)
{
    auto mutable_position = pose.mutable_position();
    mutable_position->set_x(position.x() / 1000.f + offset.x());
    mutable_position->set_y(position.y() / 1000.f + offset.y());
    mutable_position->set_z(position.z() / 1000.f + offset.z());

    auto orientation = pose.mutable_orientation();
    auto q = QQuaternion::fromAxisAndAngle(1, 0, 0, 90);
    //    const QQuaternion q{1, 0, 0, 1};
    orientation->set_w(q.scalar());
    orientation->set_x(q.x());
    orientation->set_y(q.y());
    orientation->set_z(q.z());

    width = width_ / 1000.f;
    height = height_ / 1000.f;
}

} // namespace ipme::wb
