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
        frame.screen_objects.emplace_back(Screen_object{
            screen_object.position(), offset, screen_object.size().width(),
            screen_object.size().height()});
    }

    INFO() << "Loaded " << people_count << " people and " << device_count
           << " devices " << scene_frame.screen_objects().size()
           << " screen objects";

    return frame;
}

Screen_object::Screen_object(const ipme::scene::Position& position,
                             const ipme::scene::Position& offset, double width_,
                             double height_)
{
    auto mutable_position = pose.mutable_position();
    constexpr double scale_factor = 1000.0;

    const double half_width = width_ / 2.0;
    const double position_x = (position.x() + half_width) / scale_factor;
    mutable_position->set_x(position_x + offset.x());

    const double half_height = height_ / 2.0;
    const double position_y = -(position.y() + half_height) / scale_factor;
    mutable_position->set_y(position_y + offset.y());

    mutable_position->set_z(position.z() / scale_factor + offset.z());

    const auto orientation = pose.mutable_orientation();
    const auto q = QQuaternion::fromAxisAndAngle(1, 0, 0, 90);

    orientation->set_w(q.scalar());
    orientation->set_x(q.x());
    orientation->set_y(q.y());
    orientation->set_z(q.z());

    width = width_ / scale_factor;
    height = height_ / scale_factor;
}

} // namespace ipme::wb
