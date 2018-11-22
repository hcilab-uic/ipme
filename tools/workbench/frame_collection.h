#ifndef IPME_WB_FRAME_COLLECTION_H
#define IPME_WB_FRAME_COLLECTION_H

#include <string_view>
#include <unordered_map>

#include "frame.h"
#include "protobuf/scene.pb.h"

namespace ipme::wb {
class Frame_collection {
public:
    using value_type = ipme::wb::Frame;
    using inner_container = std::vector<value_type>;
    using iterator = typename inner_container::iterator;
    using const_iterator = typename inner_container::const_iterator;

    void load(const ipme::scene::Scene& scene);

    inline iterator begin()
    {
        return std::begin(frames_);
    }

    inline iterator end()
    {
        return std::end(frames_);
    }

    inline const_iterator cbegin() const
    {
        return std::cbegin(frames_);
    }

    inline const_iterator cend() const
    {
        return std::cend(frames_);
    }

    inline const value_type& at(size_t index) const
    {
        return frames_.at(index);
    }

    inline size_t size() const
    {
        return frames_.size();
    }

    inline const value_type& operator[](size_t index) const
    {
        return frames_[index];
    }

    inline bool exists(int frame_index) const
    {
        return frame_index_map_.find(frame_index) != std::end(frame_index_map_);
    }

    size_t get_frame_id(int frame_index) const;

    void apply_filter(std::string_view filter_name);

private:
    void add(const ipme::scene::Frame& frame);

    inner_container frames_;
    std::unordered_map<std::string, ipme::scene::Display> display_map_;
    //    ipme::scene::Position screen_offset_;
    std::unordered_map<uint32_t, size_t> frame_index_map_;
    std::unordered_map<uint32_t, std::string> registered_objects_;
    std::unordered_set<uint32_t> registered_objects_ids_;
};

} // namespace ipme::wb

#endif // IPME_WB_FRAME_COLLECTION_H
