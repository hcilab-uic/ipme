#include "frame.h"

#include <QQuaternion>

#include <boost/assign.hpp>

#include "utils/logger.h"

namespace ipme::wb {
// const boost::bimap<Frame::Policy, std::string> Frame::policy_map = {
//    {Frame::Policy::fill_zeros, "Fill Zeros"},
//    {Frame::Policy::all_registered, "All Registered"},
//};

// clang-format off
const Frame::map_type Frame::policy_map =
    boost::assign::list_of<Frame::map_type::relation>
        (Frame::Policy::fill_zeros, "Fill Zeros")
        (Frame::Policy::all_registered, "All Registered");
// clang-format on

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

Frame::container Frame::load_scene_pb(const ipme::scene::Scene& scene_pb)
{
    container frames;
    const auto& config = scene_pb.config();
    std::unordered_map<uint32_t, std::string> registered_objects;
    for(const auto& object : config.registered_objects()) {
        registered_objects[object.id()] = object.name();
    }

    for(const auto& frame : scene_pb.frames()) {
        frames.emplace_back(
            create_from_pb(frame, config.screen_offset(), registered_objects));
    }
    return frames;
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
