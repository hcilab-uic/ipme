#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <unordered_map>
#include <vector>

#include "config.h"
#include "data/scene.h"
#include "display_session.h"
#include "sage_message_handler.h"

namespace ipme::wb {
class Display_manager {
public:
    Display_manager(const Config& config, std::shared_ptr<data::Scene> scene);

    bool create_sessions();

    void stop();

    void flush();

private:
    void setup_handler_map();

    Config config_;
    std::shared_ptr<data::Scene> scene_;
    boost::asio::io_context ioc_;
    std::vector<std::shared_ptr<Display_session>> sessions_;

    Display_session::handler_map_type handler_map_;
    std::shared_ptr<sage::Sage_element_container> element_container_;
};
} // namespace ipme::wb

#endif // DISPLAY_MANAGER_H
