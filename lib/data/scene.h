#ifndef IPME_DATA_SCENE_H
#define IPME_DATA_SCENE_H

#include <filesystem>
#include <memory>
#include <string_view>
#include <unordered_set>

#include "connector/omicronConnectorClient.h"
#include "protobuf/scene.pb.h"
#include "utils/logger.h"

namespace ipme {
namespace data {
class Scene : public std::enable_shared_from_this<Scene> {
public:
    using Frame_container = std::vector<std::shared_ptr<scene::Frame>>;
    using Vrpn_object = std::shared_ptr<scene::Vrpn_object>;

    virtual ~Scene();

    void set_config(const scene::Scene_config& config);

    void add_object(const omicronConnector::EventData& event);
    void add_object(std::shared_ptr<scene::Object> object);
    void add_object(uint32_t id, double top, double left, double width,
                    double height);

    void add_new_frame(uint32_t id, uint64_t timestamp);

    /// Set output file path
    void set_output_file_path(const std::filesystem::path& path);

    /// save to the scene to a file
    inline void save()
    {
        try {
            write(shared_from_this(), output_path_);
        } catch(const std::exception& ex) {
            ERROR() << "Could not complete saving the file: " << ex.what();
        }
    }

    /// Reset scene -> save scene and then clear all frames
    void reset();

    /*!
     * \brief write static convenience function that takes a scene object and an
     * output path and writes the scene to the file
     * \param scene the @c scene object to write
     * \param output_path the file path to write the scene to
     */
    static void write(std::shared_ptr<Scene> scene,
                      const std::filesystem::path& output_path);

private:
    void finalize();

    scene::Frame* current_frame_ = nullptr;
    scene::Scene scene_;
    std::filesystem::path output_path_;
    std::unordered_set<uint32_t> valid_ids_;
};
} // namespace data
} // namespace ipme

#endif // IPME_DATA_SCENE_H
