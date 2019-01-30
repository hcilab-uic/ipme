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

#ifndef IPME_WB_FRAME_H
#define IPME_WB_FRAME_H

#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/bimap.hpp>

#include "protobuf/scene.pb.h"

namespace ipme::wb {
struct Screen_object {
    double width = 0;
    double height = 0;
    ipme::scene::Pose pose;

    Screen_object(const ipme::scene::Position& position,
                  const ipme::scene::Display& config, double width_,
                  double height_);
};

class Frame {
public:
    enum class Policy : unsigned short
    {
        fill_zeros,
        all_registered
    };

    using map_type = boost::bimap<Policy, std::string>;
    using container = std::vector<Frame>;
    using motion_capture_object = ipme::scene::Vrpn_object;

    static const map_type policy_map;

    std::vector<motion_capture_object> persons;
    std::vector<motion_capture_object> devices;
    std::vector<Screen_object> screen_objects;

    inline bool has_all_registered_ids() const
    {
        return has_all_registered_ids_;
    }

    inline uint32_t frame_id() const
    {
        return frame_id_;
    }

    inline uint64_t timestamp() const
    {
        return timestamp_;
    }

    void apply_filter(Policy policy_value);
    void apply_filter(std::string_view policy_name);

    void apply_fill_zeros();
    void apply_all_registered();

    template <typename Container>
    bool has_all_vrpn_ids(Container&& ids);

    static Frame create_from_pb(
        const ipme::scene::Frame& frame,
        const std::unordered_map<std::string, scene::Display>& display_map,
        const std::unordered_map<uint32_t, std::string>& registered_objects);

    static container load_scene_pb(const ipme::scene::Scene& scene_pb);

private:
    uint32_t frame_id_{std::numeric_limits<uint32_t>::infinity()};
    uint64_t timestamp_{std::numeric_limits<uint64_t>::infinity()};
    bool has_all_registered_ids_{false};
    std::unordered_set<uint32_t> vrpn_ids_;
};
} // namespace ipme::wb

#include "frame.ipp"

#endif // IPME_WB_FRAME_H
