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
//#include "sage_messages.h"
#include "sage_session.h"
#include "utils/json.h"
#include "utils/logger.h"

namespace ipme {
namespace wb {

using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace websocket =
    boost::beast::websocket; // from <boost/beast/websocket.hpp>

constexpr std::string_view add_client_msg =
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
            "session":""
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
    : resolver_{ioc_}, /* wstream_{ioc_},*/ sage_thread_{nullptr},
      element_container_{
          std::make_shared<ipme::wb::sage::Sage_element_container>()},
      scene_{scene}, acceptor_{async_ioc_}, socket_{async_ioc_}
{
    initialize_handler_map();
}

Sage_handler::~Sage_handler()
{
    //    if(wstream_.is_open()) {
    //        wstream_.close(websocket::close_code::normal);
    //    }
}

bool Sage_handler::connect(std::string_view host, unsigned short port)
{
    //    auto results = resolver_.resolve(host, std::to_string(port));
    //    auto connect = boost::asio::connect(wstream_.next_layer(),
    //    results.begin(),
    //                                        results.end());
    //    if(connect == results.end()) {
    //        throw std::runtime_error{"Connect failed"};
    //    }

    //    // throws system_error on failure
    //    wstream_.handshake(host.data(), "/");

    //    boost::beast::multi_buffer buffer;
    //    wstream_.read(buffer);
    //    INFO() << boost::beast::buffers(buffer.data());

    //    session_ = std::make_shared<Sage_session>(async_ioc_);
    //    session_->run(host.data(), port.data(), add_client_msg.data());
    //    results->endpoint()
    const auto address = boost::asio::ip::make_address("0.0.0.0");
    boost::asio::ip::address add;
    tcp::endpoint endpoint{address, 54321};

    boost::system::error_code ec;
    acceptor_.open(endpoint.protocol(), ec);
    if(ec) {
        ERROR() << "connect(): open " << ec.category().name();
        return false;
    }

    acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
    if(ec) {
        ERROR() << "connect(): set_option " << ec.category().name();
        return false;
    }

    acceptor_.bind(endpoint, ec);
    if(ec) {
        ERROR() << "connect(): bind " << ec.category().name();
        return false;
    }

    acceptor_.listen(boost::asio::socket_base::max_connections, ec);
    if(ec) {
        ERROR() << "connect(): listen " << ec.category().name();
        return false;
    }

    return true;
}

void Sage_handler::disconnect()
{
    //    wstream_.close(websocket::close_reason{websocket::close_code::normal});
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
    if(!acceptor_.is_open()) {
        throw "acceptor is not open";
    }

    acceptor_.async_accept(socket_, std::bind(&Sage_handler::on_accept,
                                              shared_from_this(),
                                              std::placeholders::_1));

    async_ioc_.run();
}

void Sage_handler::initialize_handler_map()
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

void Sage_handler::apply_transform(const sage::Sage_element& /*element*/)
{
    // not exactly sure what this should do
}

void Sage_handler::on_accept(boost::system::error_code ec)
{
    if(ec) {
        throw "on_accept";
    }

    auto session = std::make_shared<Sage_session>(std::move(socket_));
    session->set_initial_message(add_client_msg.data());
}
} // namespace wb
} // namespace ipme
