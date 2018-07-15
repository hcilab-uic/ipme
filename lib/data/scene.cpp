#include "scene.h"

#include <fstream>
#include <memory>

#include "protobuf/scene.pb.h"

namespace ipme {
namespace data {

void Scene::add_object(const omicronConnector::EventData& event)
{
    auto object = current_frame_->add_vrpn_objects();
    object->set_timestamp(static_cast<uint64_t>(event.timestamp));
    object->set_source_id(event.sourceId);
    object->set_device_tag(event.deviceTag);
    object->set_service_type(event.serviceType);
    object->set_type(event.type);
    object->set_flags(event.flags);

    auto pose = object->mutable_pose();

    auto position = pose->mutable_position();
    position->set_x(event.posx);
    position->set_y(event.posy);
    position->set_z(event.posz);

    auto orientation = pose->mutable_orientation();
    orientation->set_w(event.orw);
    orientation->set_x(event.orx);
    orientation->set_y(event.ory);
    orientation->set_z(event.orz);
}

void Scene::add_object(std::shared_ptr<scene::Object> object)
{
    auto simple_object = current_frame_->add_simple_objects();
    simple_object->set_id(object->id());
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

void Scene::add_new_frame(uint32_t id, uint64_t timestamp)
{
    current_frame_ = scene_.add_frames();
    current_frame_->set_id(id);
    current_frame_->set_timestamp(timestamp);
}

// void Scene::set_current_frame_timestamp(uint64_t timestamp)
//{
//    if(!current_frame_) {
//        throw std::runtime_error{"Current frame is null, cannot set
//        timestamp"};
//    }

//    current_frame_->set_timestamp(timestamp);
//}

void Scene::write(std::shared_ptr<Scene> scene,
                  const std::filesystem::path& output_path)
{
    std::ofstream ofs{output_path};
    scene->scene_.SerializeToOstream(&ofs);
}

} // namespace data
} // namespace ipme
