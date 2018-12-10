#include "frame.h"

#include <QQuaternion>
#include <QVector3D>

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
    const std::unordered_map<std::string, scene::Display>& display_map,
    const std::unordered_map<uint32_t, std::string>& registered_objects)
{
    Frame frame;
    frame.frame_id_ = scene_frame.frame_id();
    frame.timestamp_ = scene_frame.timestamp();

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
                // FIXME: (Harish)
                // This should not be hard-coded. Perhaps done on a per object
                // basis, via config file or the UI
                float correction_pitch_degrees = 25.f;
                float correction_roll_degrees = 0.f;
                float correction_yaw_degrees = 0.f;
                ipme::scene::Vrpn_object corrected_object{vrpn_object};
                const auto& rot = vrpn_object.pose().orientation();
                QQuaternion corrected_rotation =
                    QQuaternion(rot.w(), rot.x(), rot.y(), rot.z()) *
                    QQuaternion::fromEulerAngles(correction_pitch_degrees,
                                                 correction_yaw_degrees,
                                                 correction_roll_degrees);
                auto corrected_rot =
                    corrected_object.mutable_pose()->mutable_orientation();
                corrected_rot->set_w(corrected_rotation.scalar());
                corrected_rot->set_x(corrected_rotation.x());
                corrected_rot->set_y(corrected_rotation.y());
                corrected_rot->set_z(corrected_rotation.z());

                frame.persons.push_back(corrected_object);
                ++people_count;
                frame.vrpn_ids_.insert(source_id);
            }
        } else if(type.find("computer") != std::string::npos) {
            if(frame.vrpn_ids_.find(source_id) == std::end(frame.vrpn_ids_)) {
                frame.devices.push_back(vrpn_object);
                ++device_count;
                frame.vrpn_ids_.insert(source_id);
            }
        } else {
            ERROR() << "Unknown type " << type;
        }
    }

    for(const auto& screen_object : scene_frame.screen_objects()) {
        const auto display = display_map.at(screen_object.screen_id());
        frame.screen_objects.emplace_back(Screen_object{
            screen_object.position(), display, screen_object.size().width(),
            screen_object.size().height()});
    }

    INFO() << "Loaded " << people_count << " people and " << device_count
           << " devices " << scene_frame.screen_objects().size()
           << " screen objects";

    bool found{true};
    for(const auto& ro : registered_objects) {
        if(ro.first == 30) {
            continue;
        }
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

namespace detail {
constexpr float to_f(double v)
{
    return static_cast<float>(v);
}

template <typename Vector>
QVector3D to_vector(Vector&& v)
{
    return QVector3D{to_f(v.x()), to_f(v.y()), to_f(v.z())};
}

} // namespace detail

Screen_object::Screen_object(const ipme::scene::Position& position,
                             const scene::Display& config, double width_,
                             double height_)
{
    auto mutable_position = pose.mutable_position();
    constexpr double scale_factor = 0.001;

    const auto offset = config.offset();

    //    const double half_width = width_ / 2.0;
    //    const double position_x = (position.x() + half_width) / scale_factor;
    //    mutable_position->set_x(position_x + offset.x());

    //    const double half_height = height_ / 2.0;
    //    const double position_y = -(position.y() + half_height) /
    //    scale_factor; mutable_position->set_y(position_y + offset.y());

    //    mutable_position->set_z(position.z() / scale_factor + offset.z());

    auto orientation = pose.mutable_orientation();

    const auto nv = config.normal_vector();
    const auto normal_direction = detail::to_vector(nv);

    static const QVector3D up{0.f, 1.f, 0.f};
    static const auto rotator = QQuaternion::fromAxisAndAngle(up, 90);

    const auto direction = rotator.rotatedVector(normal_direction);
    const auto q = QQuaternion::fromAxisAndAngle(direction, 90);

    const auto position_vector = detail::to_vector(position);
    const auto rotated = q.rotatedVector(position_vector);

    mutable_position->set_x((rotated.x() * scale_factor) + offset.x());
    mutable_position->set_y(-(rotated.y() * scale_factor) + offset.y());
    mutable_position->set_z((rotated.z() * scale_factor) + offset.z());

    DEBUG() << "quaternion for display_id " << config.display_id() << " ["
            << q.scalar() << "," << q.x() << "," << q.y() << "," << q.z()
            << "]";

    orientation->set_w(static_cast<double>(q.scalar()));
    orientation->set_x(static_cast<double>(q.x()));
    orientation->set_y(static_cast<double>(q.y()));
    orientation->set_z(static_cast<double>(q.z()));

    width = width_ * scale_factor;
    height = height_ * scale_factor;
}

} // namespace ipme::wb
