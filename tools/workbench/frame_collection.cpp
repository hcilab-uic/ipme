#include "frame_collection.h"

namespace ipme::wb {
void Frame_collection::load(const ipme::scene::Scene& scene)
{
    const auto& config = scene.config();
    screen_offset_ = config.screen_offset();

    for(const auto& object : config.registered_objects()) {
        registered_objects_[object.id()] = object.name();
    }

    for(const auto& frame : scene.frames()) {
        add(frame);
    }
}

size_t Frame_collection::get_frame_id(size_t frame_index) const
{
    if(!exists(frame_index)) {
        throw std::runtime_error{"frame index " + std::to_string(frame_index) +
                                 " not found"};
    }

    return frame_index_map_.find(frame_index)->second;
}

void Frame_collection::add(const ipme::scene::Frame& frame)
{
    frame_index_map_.emplace(frame.frame_id(), frames_.size());
    frames_.emplace_back(
        Frame::create_from_pb(frame, screen_offset_, registered_objects_));
}

} // namespace ipme::wb
