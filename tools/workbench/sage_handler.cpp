#include "sage_handler.h"

#include <string_view>

#include <boost/asio.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "sage_messages.h"

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

namespace pt = boost::property_tree;

class Json_reader {
public:
    Json_reader(std::istream& in)
    {
        pt::read_json(in, tree_);
    }

    inline std::string read(std::string_view path)
    {
        std::string ret;
        try {
            ret = tree_.get<std::string>(path.data());
        } catch(const boost::property_tree::ptree_bad_path& ex) {
            std::cout << ex.what() << std::endl;
        }

        return ret;
    }

private:
    pt::ptree tree_;
};

std::string read_json(std::istream& in, std::string_view path)
{
    pt::ptree tree;
    pt::read_json(in, tree);
    std::string ret;
    try {
        ret = tree.get<std::string>(path.data());
    } catch(const boost::property_tree::ptree_bad_path& ex) {
        std::cout << ex.what() << std::endl;
    }

    return ret;
}

void Sage_handler::internal_start()
{
    if(!wstream_.is_open()) {
        throw std::runtime_error{"Connection is not open"};
    }

    std::cout << "Sending initial message to SAGE2\n";

    if(state_machine_->is_running()) {
        wstream_.write(boost::asio::buffer(add_client_msg));

        //        wstream_.control_callback(callback);

        for(int i = 1; i < 0x9d; ++i) {
            boost::beast::multi_buffer buffer;
            wstream_.read(buffer);

            std::stringstream ss;
            ss << boost::beast::buffers(buffer.data());
            std::cout << read_json(ss, "d.listener") << std::endl;
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
        Json_reader reader{ss};
        if(reader.read("f") != "0000") {
            //"left":27,"top":40.5,"width":336,"height":182,"native_width":336,
            //"native_height":182,"previous_left":null,"previous_top":null,
            //"previous_width":null,"previous_height":null,"maximized":false,
            //"aspect":1.8461538461538463
            std::cout << "id: " << reader.read("d.id")
                      << ", title: " << reader.read("d.title")
                      << ", left: " << reader.read("d.left")
                      << ", top: " << reader.read("d.top")
                      << ", width: " << reader.read("d.width")
                      << ", height: " << reader.read("d.height")
                      << ", native_width: " << reader.read("d.native_width")
                      << ", native_height: " << reader.read("d.native_height")
                      << ", aspect: " << reader.read("d.aspect")
                      << ", maximized: " << reader.read("d.maximized") << "\n";
        }
    }

    std::cout << "Shutting down ...";
} // namespace wb
} // namespace wb
} // namespace ipme
