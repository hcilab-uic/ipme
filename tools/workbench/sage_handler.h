#ifndef IPME_WB_SAGE_HANDLER_H
#define IPME_WB_SAGE_HANDLER_H

#include <cstdlib>
#include <string_view>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

namespace ipme {
namespace wb {
class Sage_handler {
public:
    Sage_handler();

    bool connect(std::string_view host, std::string_view port);

private:
    boost::asio::io_context ioc_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> wstream_;
};

} // namespace wb
} // namespace ipme

#endif // IPME_WB_SAGE_HANDLER_H
