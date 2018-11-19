#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <vector>

#include "config.h"
#include "data/scene.h"
#include "display_session.h"

namespace ipme::wb {
class Display_manager {
public:
    Display_manager(const Config& config, std::shared_ptr<data::Scene> scene);

    bool create_sessions();

    void stop();

private:
    Config config_;
    std::shared_ptr<data::Scene> scene_;
    boost::asio::io_context ioc_;
    std::vector<std::shared_ptr<Display_session>> sessions_;
};
} // namespace ipme::wb

#endif // DISPLAY_MANAGER_H
