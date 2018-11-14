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
    static const map_type policy_map;

    std::vector<ipme::scene::Pose> persons;
    std::vector<ipme::scene::Pose> devices;
    std::vector<Screen_object> screen_objects;

    inline bool has_all_registered_ids() const
    {
        return has_all_registered_ids_;
    }

    inline uint32_t frame_id() const
    {
        return frame_id_;
    }

    void apply_filter(Policy policy_value);
    void apply_filter(std::string_view policy_name);

    void apply_fill_zeros();
    void apply_all_registered();

    template <typename Container>
    bool has_all_vrpn_ids(Container&& ids);

    static Frame create_from_pb(
        const ipme::scene::Frame& frame,
        const std::unordered_map<uint32_t, scene::Display>& display_map,
        const std::unordered_map<uint32_t, std::string>& registered_objects);

    static container load_scene_pb(const ipme::scene::Scene& scene_pb);

private:
    uint32_t frame_id_{std::numeric_limits<uint32_t>::infinity()};
    bool has_all_registered_ids_{false};
    std::unordered_set<uint32_t> vrpn_ids_;
};
} // namespace ipme::wb

#include "frame.ipp"

#endif // IPME_WB_FRAME_H
