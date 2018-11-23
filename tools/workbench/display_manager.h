#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "connector/omicronConnectorClient.h"

#include "config.h"
#include "data/scene.h"
#include "display_session.h"
#include "sage_message_handler.h"
#include "sensor/vicon_listener.h"
#include "sensor/vrpn_handler.h"

namespace ipme::wb {
class Display_manager {
public:
    Display_manager(const Config& config, std::shared_ptr<data::Scene> scene);

    bool create_sessions();

    void run();

    void stop();

    void flush();

private:
    void run_all_sessions();

    void setup_handler_map();

    Config config_;
    std::shared_ptr<data::Scene> scene_;
    boost::asio::io_context ioc_;
    std::vector<std::shared_ptr<Display_session>> display_sessions_;

    Display_session::handler_map_type handler_map_;
    std::shared_ptr<sage::Sage_element_container> element_container_;
    std::shared_ptr<std::thread> ioc_thread_;

    /// listener for the motion capture service
    sensor::Vicon_listener mc_listener_;

    using omicron = omicronConnector::OmicronConnectorClient;
    std::shared_ptr<omicron> omicron_client_;
};
} // namespace ipme::wb

#endif // DISPLAY_MANAGER_H
