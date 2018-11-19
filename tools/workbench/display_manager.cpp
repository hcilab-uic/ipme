#include "display_manager.h"

namespace ipme::wb {
Display_manager::Display_manager(const Config& config,
                                 std::shared_ptr<data::Scene> scene)
    : config_{config}, scene_{scene}
{
    for(const auto& sage_config : config_.sage_configs()) {
        sessions_.push_back(Display_session::create(ioc_, scene, sage_config));
    }
}

bool Display_manager::create_sessions()
{
    DEBUG() << "Creating sessions";
    for(auto session : sessions_) {
        session->start();
    }

    const auto count = ioc_.run();

    INFO() << "io_context count " << count;
    // TODO: We need to set some kind of return value on start/run inside
    // session started

    return true;
}

void Display_manager::stop()
{
    for(auto session : sessions_) {
        session->stop();
    }
}

} // namespace ipme::wb
