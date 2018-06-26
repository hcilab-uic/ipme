#include "sage_handler.h"

#include <string_view>

#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "sage_messages.h"
#include "utils/json.h"

namespace ipme {
namespace wb {
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
namespace websocket =
    boost::beast::websocket; // from <boost/beast/websocket.hpp>

Sage_handler::Sage_handler() : resolver_{ioc_}, wstream_{ioc_}
{
}

Sage_handler::~Sage_handler()
{
    //    if(sage_thread_) {
    //        sage_thread_->join();
    //    }

    wstream_.close(websocket::close_code::normal);
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

void callback(websocket::frame_type ft, boost::string_view sv)
{
    if(ft == websocket::frame_type::ping) {
        std::cout << "ping received\n";
    }

    std::cout << sv << "\n";
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

        //        wstream_.control_callback(callback);

        for(int i = 1; i < 0x9d; ++i) {
            boost::beast::multi_buffer buffer;
            wstream_.read(buffer);

            std::stringstream ss;
            ss << boost::beast::buffers(buffer.data());
            json.read(ss);
            std::cout << json.get("d.listener") << std::endl;
        }

        std::cout << "Sending subscribe messages\n";

        for(const auto& msg : subscribe_messages) {
            std::cout << "TX: " << msg << "\n";
            wstream_.write(boost::asio::buffer(msg));
        }
    }

    while(state_machine_->is_running()) {
        boost::beast::multi_buffer buffer;
        wstream_.read(buffer);

        std::stringstream ss;
        ss << boost::beast::buffers(buffer.data());
        json.read(ss);
        auto f_value = json.get("f");
        //        Json_reader reader{ss};
        //        auto f_value = reader.read("f");
        if(f_value != "0000") {
            //"left":27,"top":40.5,"width":336,"height":182,"native_width":336,
            //"native_height":182,"previous_left":null,"previous_top":null,
            //"previous_width":null,"previous_height":null,"maximized":false,
            //"aspect":1.8461538461538463

            // fake read
            json.get("d.id");

            if(json.last_read_success()) {
                std::cout << "id: " << json.get("d.id")
                          << ", title: " << json.get("d.title")
                          << ", left: " << json.get("d.left")
                          << ", top: " << json.get("d.top")
                          << ", width: " << json.get("d.width")
                          << ", height: " << json.get("d.height")
                          << ", native_width: " << json.get("d.native_width")
                          << ", native_height: " << json.get("d.native_height")
                          << ", aspect: " << json.get("d.aspect")
                          << ", maximized: " << json.get("d.maximized") << "\n";
            } else {
                std::cout << "f_value: " << f_value << "\n";
                std::cout << "returned value: \n" << ss.str() << "\n";
            }
        }
    }

    std::cout << "Shutting down ...";
} // namespace wb
} // namespace wb
} // namespace ipme
