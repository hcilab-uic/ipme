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

#include "frame_collection.h"

namespace ipme::wb {
void Frame_collection::load(const ipme::scene::Scene& scene)
{
    const auto& config = scene.config();
    for(const auto& display : config.displays()) {
        display_map_.emplace(display.display_id(), display);
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
