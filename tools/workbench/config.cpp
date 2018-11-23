#include "config.h"

#include <filesystem>
#include <sstream>

#include "utils/json.h"
#include "utils/logger.h"

namespace ipme::wb {
void Config::parse_config(const std::filesystem::path& path)
{
    if(!std::filesystem::exists(path)) {
        ERROR() << "config path " << path << " not found";
        return;
    }

    json_.read(path);

    auto sage_hosts = json_.get_node("sage");
    for(const auto& sage_host : sage_hosts) {
        sage_config_.emplace_back(Sage_config::create(sage_host.second));
        auto& sage_config = sage_config_.back();

        auto display = scene_config_.add_displays();
        display->set_display_id(sage_config.id);

        display->mutable_offset()->set_x(sage_config.offset.x());
        display->mutable_offset()->set_y(sage_config.offset.y());
        display->mutable_offset()->set_z(sage_config.offset.z());

        auto dim = display->mutable_dimensions();
        dim->set_height(sage_config.dimensions.height());
        dim->set_width(sage_config.dimensions.width());

        auto nv = display->mutable_normal_vector();
        nv->set_x(sage_config.normal_vector.x());
        nv->set_y(sage_config.normal_vector.y());
        nv->set_z(sage_config.normal_vector.z());

        DEBUG() << "display id " << sage_config.id << " added";
    }

    auto vrpn_objects = json_.get_node("vrpn_objects");
    for(const auto& vrpn_object : vrpn_objects) {
        auto registered_object = scene_config_.add_registered_objects();
        auto& object = vrpn_object.second;
        registered_object->set_name(object.get<std::string>("name"));
        registered_object->set_id(object.get<uint32_t>("id"));
    }

    auto vrpn_node = json_.get_node("vrpn");
    set_vrpn_host(vrpn_node.get<std::string>("host"));
    set_vrpn_port(vrpn_node.get<unsigned short>("port"));
    set_vrpn_data_port(vrpn_node.get<unsigned short>("data_port"));

    set_video_device_index(json_.get<int>("video_device_index"));
}

void Config::set_vrpn_host(const std::string& host)
{
    vrpn_host_ = host;
}

void Config::set_vrpn_port(unsigned short port)
{
    vrpn_port_ = port;
}

void Config::set_vrpn_data_port(unsigned short port)
{
    vrpn_data_port_ = port;
}

void Config::set_sage_host(const std::string& host)
{
    sage_host_ = host;
}

void Config::set_sage_port(unsigned short port)
{
    sage_port_ = port;
}

void Config::set_sage_session_token(std::string_view token)
{
    sage_session_token_ = token;
}

void Config::set_video_device_index(int index)
{
    video_device_index_ = index;
}

std::string Config::to_string() const
{
    std::stringstream ss;
    ss << "VRPN: " << vrpn_host_ << ":" << vrpn_port_ << "\n";
    ss << "Video index: " << video_device_index_ << "\n";
    ss << "Scene config:\n";
    ss << json_.to_string();

    return ss.str();
}

Config::Sage_config
Config::Sage_config::create(const boost::property_tree::ptree& sage_node)
{
    Sage_config sage_config;
    sage_config.id = sage_node.get<std::string>("id");
    sage_config.host = sage_node.get<std::string>("host");
    sage_config.port = sage_node.get<unsigned short>("port");
    sage_config.session_token = sage_node.get<std::string>("session_token");

    auto& offset_node = sage_node.get_child("offset");
    sage_config.offset.set_x(offset_node.get<double>("x"));
    sage_config.offset.set_y(offset_node.get<double>("y"));
    sage_config.offset.set_z(offset_node.get<double>("z"));

    auto& dimension_node = sage_node.get_child("dimensions");
    sage_config.dimensions.set_width(dimension_node.get<double>("width"));
    sage_config.dimensions.set_height(dimension_node.get<double>("height"));

    auto nv_node = sage_node.get_child("normal_vector");
    std::vector<double> normal_vector;
    for(const auto& nv : nv_node) {
        normal_vector.push_back(nv.second.get_value<double>());
    }

    assert(normal_vector.size() == 3);

    sage_config.normal_vector.set_x(normal_vector[0]);
    sage_config.normal_vector.set_y(normal_vector[1]);
    sage_config.normal_vector.set_z(normal_vector[2]);

    return sage_config;
}

} // namespace ipme::wb
