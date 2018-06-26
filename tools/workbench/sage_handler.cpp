#include "sage_handler.h"

#include <memory>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "sage/create_app_window_handler.h"
#include "sage/default_sage_message_handler.h"
#include "sage/update_item_order_handler.h"
#include "sage_message_handler.h"
#include "sage_messages.h"
#include "utils/json.h"

namespace ipme {
namespace wb {
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

Sage_handler::Sage_handler()
    : resolver_{ioc_}, wstream_{ioc_},
      element_container_{
          std::make_shared<ipme::wb::sage::Sage_element_container>()}
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
    //    handler_map_.insert(
    //        create_default("0018", "updateApplicationStateOptions"));
    //    handler_map_.insert(create_default("0019", "requestClientUpdate"));
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

bool Sage_handler::connect(std::string_view host, std::string_view port)
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
    wstream_.read(buffer);
    std::cout << boost::beast::buffers(buffer.data()) << std::endl;

    return true;
}

void Sage_handler::start()
{
    sage_thread_ =
        std::make_unique<std::thread>(&Sage_handler::internal_start, this);
}

void Sage_handler::set_state_machine(
    std::shared_ptr<State_machine> state_machine)
{
    state_machine_ = state_machine;
}

void Sage_handler::internal_start()
{
    if(!wstream_.is_open()) {
        throw std::runtime_error{"Connection is not open"};
    }

    std::cout << "Sending initial message to SAGE2\n";

    utils::Json json;
    if(state_machine_->is_running()) {
        wstream_.write(boost::asio::buffer(add_client_msg));

        for(int i = 1; i < 0x9d; ++i) {
            boost::beast::multi_buffer buffer;
            wstream_.read(buffer);

            std::stringstream ss;
            ss << boost::beast::buffers(buffer.data());
            json.read(ss);
            std::cout << json.get("d.listener") << std::endl;
        }

        std::cout << "Sending subscribe messages\n";

        for(const auto& handler : handler_map_) {
            const auto msg = handler.second->generate_registration_message();
            std::cout << "TX: " << msg;
            wstream_.write(boost::asio::buffer(msg));
        }
    }

    while(state_machine_->is_running()) {
        boost::beast::multi_buffer buffer;
        wstream_.read(buffer);

        std::stringstream ss;
        ss << boost::beast::buffers(buffer.data());
        json.read(ss);
        const auto& handler = handler_map_[json.get("f")];
        handler->dispatch(json);
    }

    std::cout << "Shutting down ...";
} // namespace wb
} // namespace wb
} // namespace ipme
