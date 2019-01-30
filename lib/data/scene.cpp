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

#include "scene.h"

#include <fstream>
#include <memory>
#include <sstream>

#include "google/protobuf/util/json_util.h"
#include "protobuf/scene.pb.h"
#include "utils/logger.h"

namespace ipme {
namespace data {

Scene::~Scene()
{
    finalize();
}

void Scene::set_config(const scene::Scene_config& config)
{
    scene_.mutable_config()->CopyFrom(config);

    std::string config_string;
    google::protobuf::util::JsonPrintOptions print_options;
    print_options.add_whitespace = true;
    google::protobuf::util::MessageToJsonString(scene_.config(), &config_string,
                                                print_options);

    valid_ids_.clear();
    for(const auto& object : scene_.config().registered_objects()) {
        valid_ids_.insert(object.id());
    }

    //    INFO() << "New scene config\n" << config_string;
}

void Scene::add_object(const omicronConnector::EventData& event)
{
    if(!current_frame_) {
        // This means that the first video frame has not been initialized. We
        // shall wait until that has been done
        TRACE() << "current_frame_ not initialized. Not updating.";
        return;
    }

    if(valid_ids_.find(event.sourceId) == std::end(valid_ids_)) {
        WARN() << "Unregistered VRPN device ID " << event.sourceId
               << ", ignoring";
    }

    auto object = current_frame_->add_vrpn_objects();
    object->set_timestamp(static_cast<uint64_t>(event.timestamp));
    object->set_vrpn_source_id(event.sourceId);
    object->set_device_tag(event.deviceTag);
    object->set_service_type(event.serviceType);
    object->set_type(event.type);
    object->set_flags(event.flags);

    auto pose = object->mutable_pose();

    auto position = pose->mutable_position();
    position->set_x(static_cast<double>(event.posx));
    position->set_y(static_cast<double>(event.posy));
    position->set_z(static_cast<double>(event.posz));

    auto orientation = pose->mutable_orientation();
    orientation->set_w(static_cast<double>(event.orw));
    orientation->set_x(static_cast<double>(event.orx));
    orientation->set_y(static_cast<double>(event.ory));
    orientation->set_z(static_cast<double>(event.orz));

    DEBUG() << "Added object with source id: " << event.sourceId
            << ", position " << position->DebugString() << ", quaternion "
            << orientation->DebugString();
}

void Scene::add_object(std::shared_ptr<scene::Object> object)
{
    if(!current_frame_) {
        return;
    }

    // There is lot of duplicate work here. Please fix
    auto simple_object = current_frame_->add_simple_objects();
    simple_object->set_object_id(object->object_id());
    simple_object->set_name(object->name());
    simple_object->set_timestamp(object->timestamp());

    auto pose = simple_object->mutable_pose();

    auto position = pose->mutable_position();
    position->set_x(simple_object->pose().position().x());
    position->set_y(simple_object->pose().position().y());
    position->set_z(simple_object->pose().position().z());

    auto orientation = pose->mutable_orientation();
    orientation->set_w(simple_object->pose().orientation().w());
    orientation->set_x(simple_object->pose().orientation().x());
    orientation->set_y(simple_object->pose().orientation().y());
    orientation->set_z(simple_object->pose().orientation().z());
}

void Scene::add_object(const std::string& id, const std::string& display_id,
                       double top, double left, double width, double height)
{
    if(!current_frame_) {
        return;
    }

    auto screen_object = current_frame_->add_screen_objects();

    screen_object->set_screen_object_id(id);
    screen_object->set_screen_id(display_id);

    auto position = screen_object->mutable_position();
    position->set_x(left);
    position->set_y(top);
    position->set_z(0);

    auto size = screen_object->mutable_size();
    size->set_width(width);
    size->set_height(height);

    DEBUG() << "Screen object ID:" << id << " T:" << top << " L:" << left
            << " W:" << width << " H:" << height;
}

void Scene::add_new_frame(uint32_t id, uint64_t timestamp)
{
    current_frame_ = scene_.add_frames();
    current_frame_->set_frame_id(id);
    current_frame_->set_timestamp(timestamp);
    DEBUG() << "Current frame id set to " << id << " timestamp set to "
            << timestamp;
}

void Scene::set_output_file_path(const std::filesystem::__cxx11::path& path)
{
    output_path_ = path;
    INFO() << "Writing scene output to " << output_path_;
}

void Scene::reset()
{
    save();
    scene_.clear_frames();
}

void Scene::write(std::shared_ptr<Scene> scene,
                  const std::filesystem::path& output_path)
{
    auto dirname = output_path.parent_path();
    std::filesystem::create_directories(dirname);

    std::ofstream ofs{output_path,
                      std::ios::out | std::ios::trunc | std::ios::binary};

    DEBUG() << "Trying to write " << scene->scene_.ByteSize() << " bytes to "
            << output_path;

    if(ofs.is_open()) {
        DEBUG() << "Opened stream to " << output_path;
    } else {
        ERROR() << "Could not open stream to " << output_path;
    }

    if(scene->scene_.SerializeToOstream(&ofs)) {
        INFO() << "Wrote output to " << output_path;
    } else {
        WARN() << "Could not write " << output_path;
    }
}

Scene::Display_map Scene::build_display_map(const scene::Scene& scene)
{
    Display_map map;
    for(const auto& display : scene.config().displays()) {
        map.emplace(display.display_id(), display);
    }

    return map;
}

Scene Scene::load(std::ifstream& ifs)
{
    ipme::scene::Scene scene_pb;
    scene_pb.ParseFromIstream(&ifs);

    Scene scene;
    scene.display_map_ = build_display_map(scene_pb);

    return scene;
}

void Scene::finalize()
{
    save();
}

} // namespace data
} // namespace ipme
