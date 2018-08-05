#ifndef IPME_WB_CONFIG_H
#define IPME_WB_CONFIG_H

#include <filesystem>
#include <string.h>

#include "protobuf/scene.pb.h"
#include "utils/json.h"

namespace ipme::wb {
class Config {
public:
    inline Config(const std::filesystem::path& path)
    {
        parse_config(path);
    }

    inline std::string vrpn_host() const
    {
        return vrpn_host_;
    }

    inline short vrpn_port() const
    {
        return vrpn_port_;
    }

    inline short vrpn_data_port() const
    {
        return vrpn_data_port_;
    }

    inline std::string sage_host() const
    {
        return sage_host_;
    }

    inline short sage_port() const
    {
        return sage_port_;
    }

    inline int video_device_index() const
    {
        return video_device_index_;
    }

    inline bool vrpn_on() const
    {
        return vrpn_on_;
    }

    inline bool sage_on() const
    {
        return sage_on_;
    }

    inline bool video_on() const
    {
        return video_on_;
    }

    const scene::Scene_config& scene_config() const
    {
        return scene_config_;
    }

    void set_screen_offset(double x, double y, double z);

    void set_vrpn_host(const std::string& host);
    void set_vrpn_port(short port);
    void set_vrpn_data_port(short port);

    void set_sage_host(const std::string& host);
    void set_sage_port(short port);

    void set_video_device_index(int index);

    std::string to_string() const;

private:
    void parse_config(const std::filesystem::path& path);

    void create_default_config();

    scene::Scene_config scene_config_;
    utils::Json json_;

    std::string vrpn_host_ = "thor.evl.uic.edu";
    short vrpn_port_ = 28000;
    short vrpn_data_port_ = 7000;

    std::string sage_host_ = "localhost";
    short sage_port_ = 9292;

    int video_device_index_ = 0;

    bool vrpn_on_ = true;
    bool sage_on_ = true;
    bool video_on_ = true;
};

} // namespace ipme::wb
#endif // IPME_WB_CONFIG_H
