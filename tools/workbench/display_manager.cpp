#include "display_manager.h"

#include "sage/create_app_window_handler.h"
#include "sage/default_sage_message_handler.h"
#include "sage/update_item_order_handler.h"
#include "sage_message_handler.h"

namespace ipme::wb {
Display_manager::Display_manager(const Config& config,
                                 std::shared_ptr<data::Scene> scene)
    // clang-format off
    : config_{config}
    , scene_{scene}
    , element_container_{std::make_shared<sage::Sage_element_container>()}
// clang-format on
{
    setup_handler_map();
    for(const auto& sage_config : config_.sage_configs()) {
        sessions_.push_back(
            Display_session::create(ioc_, scene, sage_config, handler_map_));
    }
}

bool Display_manager::create_sessions()
{
    DEBUG() << "Creating sessions";
    for(auto session : sessions_) {
        session->start();
    }

    // TODO: We need to set some kind of return value on start/run inside
    // session started

    return true;
}

void Display_manager::start()
{
    INFO() << "starting the run sequence";
    //    const auto count = ioc_.run();
    //    INFO() << count << " contexts created";
    ioc_thread_ = std::make_shared<std::thread>([this] { ioc_.run(); });
}

void Display_manager::stop()
{
    for(auto session : sessions_) {
        session->stop();
    }
}

void Display_manager::flush()
{
    for(const auto& element : element_container_->elements()) {
        char* end;
        auto& ui = element.second;
        uint32_t id =
            static_cast<uint32_t>(std::strtoul(ui.id().data(), &end, 10));
        scene_->add_object(id, ui.top(), ui.left(), ui.width(), ui.height());
    }
}

std::pair<std::string, std::shared_ptr<Sage_message_handler>>
create_default(const std::string& alias, const std::string& name)
{
    using handler_type = ipme::wb::sage::Default_sage_message_handler;
    return std::make_pair(alias, std::make_shared<handler_type>(name, alias));
}

template <typename Handler>
std::pair<std::string, std::shared_ptr<Sage_message_handler>>
create(const std::string& alias)
{
    return std::make_pair(alias, std::make_shared<Handler>(alias));
}

void Display_manager::setup_handler_map()
{
    namespace sage = ipme::wb::sage;
    handler_map_.insert(create_default("0000", "partitionsGrabAllContent"));
    handler_map_.insert(create_default("0007", "appResize"));
    handler_map_.insert(create_default("0008", "appFullscreen"));
    handler_map_.insert(create_default("0009", "clearDisplay"));
    handler_map_.insert(create<sage::Create_app_window_handler>("000a"));
    handler_map_.insert(create<sage::Update_item_order_handler>("000b"));
    handler_map_.insert(create_default("000c", "deleteAllApplications"));
    handler_map_.insert(create_default("000d", "tileApplications"));
    handler_map_.insert(create_default("0018", "startSagePointer"));
    handler_map_.insert(create_default("0019", "createSagePointer"));
    handler_map_.insert(create_default("001a", "setItemPosition"));
    handler_map_.insert(create_default("001b", "setItemPositionAndSize"));
    handler_map_.insert(create_default("001c", "deleteElement"));

    for(auto& handler : handler_map_) {
        handler.second->set_element_container(element_container_);
    }
}

} // namespace ipme::wb
