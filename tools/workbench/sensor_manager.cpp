#include "sensor_manager.h"

#include "connector/omicronConnectorClient.h"

#include "sage/create_app_window_handler.h"
#include "sage/default_sage_message_handler.h"
#include "sage/update_item_order_handler.h"
#include "sage_message_handler.h"
#include "sensor/vrpn_handler.h"

namespace ipme::wb {
Sensor_manager::Sensor_manager(const Config& config,
                               std::shared_ptr<data::Scene> scene)
    // clang-format off
    : config_{config}
    , scene_{scene}
    , element_container_{std::make_shared<sage::Sage_element_container>()}
    , mc_listener_{std::make_unique<sensor::Vrpn_handler>(scene)}
// clang-format on
{
    setup_handler_map();
    for(const auto& sage_config : config_.sage_configs()) {
        display_sessions_.push_back(
            Display_session::create(ioc_, scene, sage_config, handler_map_));
    }
}

Sensor_manager::~Sensor_manager()
{
    stop();
}

bool Sensor_manager::create_sessions()
{
    DEBUG() << "Creating sessions";

    bool session_created{true};
    for(auto session : display_sessions_) {
        session_created &= session->initialize() & session_created;
    }

    // TODO: We need to set some kind of return value on start/run inside
    // session started

    omicron_client_ = std::make_shared<omicron>(&mc_listener_);
    std::stringstream ss;
    INFO() << "Connecting to motion capture service " << config_.vrpn_host()
           << ":" << config_.vrpn_port();
    session_created &=
        omicron_client_->connect(config_.vrpn_host().c_str(),
                                 config_.vrpn_port(), config_.vrpn_data_port());

    return session_created;
}

void Sensor_manager::run()
{
    INFO() << "starting the run sequence";
    //    const auto count = ioc_.run();
    //    INFO() << count << " contexts created";
    ioc_thread_ = std::make_shared<std::thread>([this] { ioc_.run(); });
}

void Sensor_manager::stop()
{
    for(auto session : display_sessions_) {
        session->stop();
    }

    if(ioc_thread_) {
        ioc_thread_->join();
        INFO() << "sessions stopped, disposing omicron connection";

        omicron_client_->dispose();
    }
}

void Sensor_manager::flush()
{
    for(const auto& element : element_container_->elements()) {
        // clang-format off
        scene_->add_object(element.second.id(),
                           element.second.display_id(),
                           element.second.top(),
                           element.second.left(),
                           element.second.width(),
                           element.second.height());
        // clang-format on
    }

    INFO() << element_container_->elements().size() << " elements flushed";
}

void Sensor_manager::run_all_sessions()
{
    ioc_.run();

    if(omicron_client_) {
        // Currently we don't have a defined way of stopping this loop. They way
        // we do it is, call stop for the display sessions on the original
        // thread and wait for them to stop. When they are stopped, we can
        // assume our work is done and exit loop. A better way to do this would
        // be to make VRPN session compliant with display sessions
        while(!ioc_.stopped()) {
            omicron_client_->poll();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }

        INFO() << "Stop condition reached for omicron, exiting loop";
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

void Sensor_manager::setup_handler_map()
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
