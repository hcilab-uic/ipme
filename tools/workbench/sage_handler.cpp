#include "sage_handler.h"

#include <boost/asio.hpp>

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
    if(sage_thread_) {
        sage_thread_->join();
    }

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

void Sage_handler::internal_start()
{
    std::cout << "Sending initial message to SAGE2\n";
    std::string message{"{}"};
    while(state_machine_->state() == State_machine::State::running) {
        wstream_.write(boost::asio::buffer(message));
        boost::beast::multi_buffer buffer;
        wstream_.read(buffer);
    }
}
} // namespace wb
} // namespace ipme
