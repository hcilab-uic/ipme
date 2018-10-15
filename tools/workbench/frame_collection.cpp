#include "frame_collection.h"

namespace ipme::wb {
void Frame_collection::load(const ipme::scene::Scene& scene)
{
    const auto& config = scene.config();
    //    screen_offset_ = config.screen_offset();

    //    for(const auto& screen : config.screens()) {
    //        display_map_.emplace(screen.id(), screen.position());
    //    }

    for(const auto& display : config.displays()) {
        display_map_.emplace(display.display_id(), display);
    }

    for(const auto& screen : config.screens()) {
        display_map_.emplace(screen.id(), screen.position());
    }

    for(const auto& object : config.registered_objects()) {
        registered_objects_[object.id()] = object.name();
        registered_objects_ids_.insert(object.id());
    }

    for(const auto& frame : scene.frames()) {
        add(frame);
    }
}

size_t Frame_collection::get_frame_id(int frame_index) const
{
    if(!exists(frame_index)) {
        throw std::runtime_error{"frame index " + std::to_string(frame_index) +
                                 " not found"};
    }

    return frame_index_map_.find(frame_index)->second;
}

void Frame_collection::apply_filter(std::string_view filter_name)
{
    const auto filter =
        Frame::policy_map.right.at(std::string{filter_name.data()});
    if(filter == Frame::Policy::all_registered) {
        auto itr = std::begin(frames_);
        const auto itr_end = std::end(frames_);
        while(itr != itr_end) {
            if(!itr->has_all_vrpn_ids(registered_objects_ids_)) {
                std::cout << "removing \n";
                itr = frames_.erase(itr);
            }
        }
    } else {
        for(auto& frame : frames_) {
            frame.apply_filter(filter_name);
        }
    }
}

void Frame_collection::add(const ipme::scene::Frame& frame)
{
    frame_index_map_.emplace(frame.frame_id(), frames_.size());
    frames_.emplace_back(
        Frame::create_from_pb(frame, display_map_, registered_objects_));
}

} // namespace ipme::wb
