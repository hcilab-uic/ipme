#ifndef IPME_DATA_SCENE_H
#define IPME_DATA_SCENE_H

#include <filesystem>
#include <memory>
#include <string_view>

#include "connector/omicronConnectorClient.h"
#include "protobuf/scene.pb.h"

namespace ipme {
namespace data {
class Scene : public std::enable_shared_from_this<Scene> {
public:
    using Frame_container = std::vector<std::shared_ptr<scene::Frame>>;
    using Vrpn_object = std::shared_ptr<scene::Vrpn_object>;

    void add_object(const omicronConnector::EventData& event);
    void add_object(std::shared_ptr<scene::Object> object);

    void add_new_frame(uint32_t id, uint64_t timestamp);
    //    void set_current_frame_timestamp(uint64_t timestamp);

    inline void set_output_file_path(const std::filesystem::path& path)
    {
        output_path_ = path;
    }

    inline void save()
    {
        write(shared_from_this(), output_path_);
    }

    static void write(std::shared_ptr<Scene> scene,
                      const std::filesystem::path& output_path);

private:
    scene::Frame* current_frame_ = nullptr;
    scene::Scene scene_;
    std::filesystem::path output_path_;
};
} // namespace data
} // namespace ipme

#endif // IPME_DATA_SCENE_H
