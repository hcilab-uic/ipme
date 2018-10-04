#ifndef IPME_WB_FRAME_H
#define IPME_WB_FRAME_H

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/bimap.hpp>

#include "protobuf/scene.pb.h"

namespace ipme::wb {
struct Screen_object {
    float width = 0;
    float height = 0;
    ipme::scene::Pose pose;

    Screen_object(const ipme::scene::Position& position,
                  const ipme::scene::Position& offset, double width_,
                  double height_);
};

struct Frame {
    enum class Policy : unsigned short
    {
        fill_zeros,
        all_registered
    };

    using map_type = boost::bimap<Policy, std::string>;
    using container = std::vector<Frame>;
    static const map_type policy_map;

    std::vector<ipme::scene::Pose> persons;
    std::vector<ipme::scene::Pose> devices;
    std::vector<Screen_object> screen_objects;

    static Frame create_from_pb(
        const ipme::scene::Frame& frame, const ipme::scene::Position& offset,
        const std::unordered_map<uint32_t, std::string>& registered_objects);

    static container load_scene_pb(const ipme::scene::Scene& scene_pb);
};

} // namespace ipme::wb

#endif // IPME_WB_FRAME_H
