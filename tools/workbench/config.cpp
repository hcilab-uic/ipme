#include "config.h"

#include <filesystem>
#include <sstream>

#include "utils/json.h"

namespace ipme::wb {
void Config::parse_config(const std::filesystem::path& path)
{
    if(!std::filesystem::exists(path)) {
        return;
    }

    json_.read(path);

    set_screen_offset(json_.get<double>("screen_offsets.x"),
                      json_.get<double>("screen_offsets.y"),
                      json_.get<double>("screen_offsets.z"));

    auto vrpn_objects = json_.get_node("vrpn_objects");
    for(const auto& vrpn_object : vrpn_objects) {
        auto registered_object = scene_config_.add_registered_objects();
        auto& object = vrpn_object.second;
        registered_object->set_name(object.get<std::string>("name"));
        registered_object->set_id(object.get<uint32_t>("id"));
    }

    auto vrpn_node = json_.get_node("vrpn");
    set_vrpn_host(vrpn_node.get<std::string>("host"));
    set_vrpn_port(vrpn_node.get<short>("port"));
    set_vrpn_data_port(vrpn_node.get<short>("data_port"));

    auto sage_node = json_.get_node("sage");
    set_sage_host(sage_node.get<std::string>("host"));
    set_sage_port(sage_node.get<short>("port"));
    set_sage_session_token(sage_node.get<std::string>("session_token"));

    set_video_device_index(json_.get<int>("video_device_index"));
}

void Config::create_default_config()
{
    set_screen_offset(3.6, 2.3, 3.6);
}

void Config::set_screen_offset(double x, double y, double z)
{
    auto screen_offset = scene_config_.mutable_screen_offset();

    screen_offset->set_x(x);
    screen_offset->set_y(y);
    screen_offset->set_z(z);
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
    ss << "SAGE: " << sage_host_ << ":" << sage_port_ << "\n";
    ss << "Video index: " << video_device_index_ << "\n";
    ss << "Scene config:\n";
    ss << json_.to_string();

    return ss.str();
}
} // namespace ipme::wb
