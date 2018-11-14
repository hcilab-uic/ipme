#include "frame.h"

#include <QQuaternion>

#include <boost/assign.hpp>

#include "utils/logger.h"

namespace ipme::wb {
// clang-format off
const Frame::map_type Frame::policy_map =
    boost::assign::list_of<Frame::map_type::relation>
        (Frame::Policy::fill_zeros, "Fill Zeros")
        (Frame::Policy::all_registered, "All Registered");
// clang-format on

/* static */ void Frame::apply_filter(Frame::Policy policy_value)
{
    switch(policy_value) {
    case Policy::fill_zeros:
        return apply_fill_zeros();

    case Policy::all_registered:
        return apply_all_registered();

    default:
        throw std::runtime_error{"invalid policy"};
    }
}

void Frame::apply_filter(std::string_view policy_name)
{
    apply_filter(policy_map.right.at(std::string{policy_name.data()}));
}

void Frame::apply_fill_zeros()
{
}

void Frame::apply_all_registered()
{
    throw std::runtime_error{"apply_all_registered() unimplemented"};
}

Frame Frame::create_from_pb(
    const ipme::scene::Frame& scene_frame,
    const std::unordered_map<uint32_t, ipme::scene::Display>& display_map,
    const std::unordered_map<uint32_t, std::string>& registered_objects)
{
    Frame frame;
    frame.frame_id_ = scene_frame.frame_id();

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
            if(frame.vrpn_ids_.find(source_id) == std::end(frame.vrpn_ids_)) {
                frame.persons.push_back(vrpn_object.pose());
                ++people_count;
                frame.vrpn_ids_.insert(source_id);
            }
        } else if(type.find("computer") != std::string::npos) {
            if(frame.vrpn_ids_.find(source_id) == std::end(frame.vrpn_ids_)) {
                frame.devices.push_back(vrpn_object.pose());
                ++device_count;
                frame.vrpn_ids_.insert(source_id);
            }
        } else {
            ERROR() << "Unknown type " << type;
        }
    }

    for(const auto& screen_object : scene_frame.screen_objects()) {
        const auto& display = display_map.at(screen_object.screen_id());
        frame.screen_objects.emplace_back(Screen_object{
            screen_object.position(), display, screen_object.size().width(),
            screen_object.size().height()});
    }

    INFO() << "Loaded " << people_count << " people and " << device_count
           << " devices " << scene_frame.screen_objects().size()
           << " screen objects";

    bool found{true};
    for(const auto& ro : registered_objects) {
        if(frame.vrpn_ids_.find(ro.first) == std::end(frame.vrpn_ids_)) {
            found = false;
            break;
        }
    }

    frame.has_all_registered_ids_ = found;

    return frame;
}

// Frame::container Frame::load_scene_pb(const ipme::scene::Scene& scene_pb)
//{
//    container frames;
//    const auto& config = scene_pb.config();
//    std::unordered_map<uint32_t, std::string> registered_objects;
//    for(const auto& object : config.registered_objects()) {
//        registered_objects[object.id()] = object.name();
//    }

//    for(const auto& frame : scene_pb.frames()) {
//        frames.emplace_back(create_from_pb(frame, config,
//        registered_objects));
//    }
//    return frames;
//}

Screen_object::Screen_object(const ipme::scene::Position& position,
                             const scene::Display& config, double width_,
                             double height_)
{
    auto mutable_position = pose.mutable_position();
    constexpr double scale_factor = 1000.0;

    const auto offset = config.offset();

    const double half_width = width_ / 2.0;
    const double position_x = (position.x() + half_width) / scale_factor;
    mutable_position->set_x(position_x + offset.x());

    const double half_height = height_ / 2.0;
    const double position_y = -(position.y() + half_height) / scale_factor;
    mutable_position->set_y(position_y + offset.y());

    mutable_position->set_z(position.z() / scale_factor + offset.z());

    const auto orientation = pose.mutable_orientation();
    const auto q = QQuaternion::fromAxisAndAngle(1, 0, 0, 90);

    orientation->set_w(static_cast<double>(q.scalar()));
    orientation->set_x(static_cast<double>(q.x()));
    orientation->set_y(static_cast<double>(q.y()));
    orientation->set_z(static_cast<double>(q.z()));

    width = width_ / scale_factor;
    height = height_ / scale_factor;
}

} // namespace ipme::wb
