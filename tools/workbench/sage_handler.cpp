#include "sage_handler.h"

#include <memory>
#include <regex>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "sage/create_app_window_handler.h"
#include "sage/default_sage_message_handler.h"
#include "sage/update_item_order_handler.h"
#include "sage_message_handler.h"
//#include "sage_messages.h"
#include "sage_session.h"
#include "utils/json.h"
#include "utils/logger.h"

namespace ipme {
namespace wb {
constexpr const char* add_client_msg_template =
    R"(
{
    "f":"0001",
    "d":
    {
        "clientType":"display",
        "requests":
        {
            "config":true,
            "version":true,
            "time":true,
            "console":false
        },
        "clientID":"0",
        "browser":"",
        "session":"%s"
    }
}
)";

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace websocket =
    boost::beast::websocket; // from <boost/beast/websocket.hpp>

std::pair<std::string_view, std::shared_ptr<Sage_message_handler>>
create_default(std::string_view alias, std::string_view name)
{
    return std::make_pair(
        alias, std::make_shared<ipme::wb::sage::Default_sage_message_handler>(
                   name, alias));
}

template <typename Handler>
std::pair<std::string_view, std::shared_ptr<Sage_message_handler>>
create(std::string_view alias)
{
    return std::make_pair(alias, std::make_shared<Handler>(alias));
}

Sage_handler::Sage_handler(std::shared_ptr<data::Scene> scene)
    : resolver_{ioc_}, wstream_{ioc_}, sage_thread_{nullptr},
      element_container_{
          std::make_shared<ipme::wb::sage::Sage_element_container>()},
      scene_{scene}
{
    namespace sage = ipme::wb::sage;
    //    handler_map_.insert(create_default("0002",
    //    "registerInteractionClient"));
    //    handler_map_.insert(create_default("0003", "getActiveClients"));
    //    handler_map_.insert(create_default("0004", "getRbac"));
    //    handler_map_.insert(create_default("0005", "updateAppState"));
    //    handler_map_.insert(create_default("0006", "updateStateOptions"));
    handler_map_.insert(create_default("0007", "appResize"));
    handler_map_.insert(create_default("0008", "appFullscreen"));
    handler_map_.insert(create_default("0009", "clearDisplay"));

    handler_map_.insert(create<sage::Create_app_window_handler>("000a"));
    //    handler_map_.insert(create<sage::Update_item_order_handler>("000b"));

    handler_map_.insert(create_default("000c", "deleteAllApplications"));
    handler_map_.insert(create_default("000d", "tileApplications"));
    //    handler_map_.insert(create_default("000e", "addNewWebElement"));
    //    handler_map_.insert(create_default("000f", "openNewWebpage"));
    //    handler_map_.insert(create_default("0010", "startApplicationMove"));
    //    handler_map_.insert(create_default("0011", "startApplicationResize"));
    //    handler_map_.insert(create_default("0012",
    //    "updateApplicationPosition")); handler_map_.insert(
    //        create_default("0013", "updateApplicationPositionAndSize"));
    //    handler_map_.insert(create_default("0014", "finishApplicationMove"));
    //    handler_map_.insert(create_default("0015",
    //    "finishApplicationResize"));
    //    handler_map_.insert(create_default("0016", "deleteApplication"));
    //    handler_map_.insert(create_default("0017", "updateApplicationState"));
    handler_map_.insert(create_default("0018", "startSagePointer"));
    handler_map_.insert(create_default("0019", "createSagePointer"));
    handler_map_.insert(create_default("0020", "setItemPosition"));
    handler_map_.insert(create_default("0021", "setItemPositionAndSize"));
    handler_map_.insert(create_default("0022", "deleteElement"));

    for(auto& handler : handler_map_) {
        handler.second->set_element_container(element_container_);
    }
}

Sage_handler::~Sage_handler()
{
    if(wstream_.is_open()) {
        wstream_.close(websocket::close_code::normal);
    }
}

bool Sage_handler::connect(std::string_view host, std::string_view port,
                           std::string_view session_token)
{
    auto results = resolver_.resolve(host, port);
    auto connect = boost::asio::connect(wstream_.next_layer(), results.begin(),
                                        results.end());
    if(connect == results.end()) {
        throw std::runtime_error{"Connect failed"};
    }

    // throws system_error on failure
    wstream_.handshake(host.data(), "/");

    boost::beast::multi_buffer buffer;
    DEBUG() << "Doing first SAGE read";
    wstream_.read(buffer);
    INFO() << boost::beast::buffers(buffer.data());

    //    session_ = std::make_shared<Sage_session>(async_ioc_);
    //    session_->run(host.data(), port.data(), add_client_msg.data());
    int size = std::snprintf(nullptr, 0, add_client_msg_template,
                             session_token.data());
    if(size < 0) {
        throw std::runtime_error{"error in adding token"};
    }

    std::vector<char> buf(static_cast<size_t>(size + 1));
    std::snprintf(&buf[0], buf.size(), add_client_msg_template,
                  session_token.data());
    std::string add_client_msg{std::begin(buf), std::end(buf) - 1};

    if(!wstream_.is_open()) {
        throw std::runtime_error{"Connection is not open"};
    }

    DEBUG() << "Sending add client message: " << add_client_msg;

    //    if(state_machine_->is_running()) {
    wstream_.write(boost::asio::buffer(add_client_msg));

    utils::Json json;
    // FIXME: 0x9d is hardcoded. Find a way to do this dynamically
    for(int i = 1; i < 0x9d; ++i) {
        boost::beast::multi_buffer buffer;
        try {
            wstream_.read(buffer);
        } catch(const boost::system::system_error& err) {
            ERROR() << err.what();
            return false;
        }

        std::stringstream ss;
        ss << boost::beast::buffers(buffer.data());
        json.read(ss);
        INFO() << "RX: " << json.get("d.listener");
    }

    INFO() << "Sending subscribe messages";

    for(const auto& handler : handler_map_) {
        const auto msg = handler.second->generate_registration_message();
        INFO() << "TX: " << msg;
        wstream_.write(boost::asio::buffer(msg));
    }
    //    } else {
    //        WARN() << "State machine is not running, returning";
    //        return false;
    //    }

    return true;
}

void Sage_handler::disconnect()
{
    wstream_.close(websocket::close_reason{websocket::close_code::normal});
}

bool Sage_handler::start()
{
    if(!sage_thread_) {
        sage_thread_ =
            std::make_unique<std::thread>(&Sage_handler::internal_start, this);

        return sage_thread_ != nullptr;
    }

    return false;
}

void Sage_handler::stop()
{
    //    DEBUG() << "Joining SAGE2 thread to exit";
    //    sage_thread_->join();
    //    DEBUG() << "SAGE2 thread exited";
    sage_thread_.release();

    if(!sage_thread_) {
        DEBUG() << "SAGE2 thread resources released";
    } else {
        ERROR() << "Did not release SAGE2 thread resources";
    }
}

void Sage_handler::set_state_machine(
    std::shared_ptr<State_machine> state_machine)
{
    state_machine_ = state_machine;
}

void Sage_handler::flush()
{
    const auto elements = element_container_->elements();
    for(const auto& element : elements) {
        char* end;
        auto& ui = element.second;
        size_t id = std::strtoul(ui.id().data(), &end, 10);
        scene_->add_object(id, ui.top(), ui.left(), ui.width(), ui.height());
        // FIXME: blah blah

        //        apply_transform(element.second);
        //        scene_->add_object(nullptr);
    }

    DEBUG() << elements.size() << " SAGE2 elements flushed";
}

void Sage_handler::internal_start()
{
    //    async_ioc_.run();

    while(state_machine_->is_running()) {
        boost::beast::multi_buffer buffer;
        wstream_.read(buffer);

        std::stringstream ss;
        ss << boost::beast::buffers(buffer.data());

        utils::Json json;
        json.read(ss);
        const auto& handler = handler_map_[json.get("f")];
        handler->dispatch(json);
    }

    INFO() << "Shutting down SAGE2 handler...";
}

void Sage_handler::apply_transform(const sage::Sage_element& /*element*/)
{
    // not exactly sure what this should do
} // namespace wb
} // namespace wb
} // namespace ipme
