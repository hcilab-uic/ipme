#ifndef IPME_WB_SAGE_SESSION_H
#define IPME_WB_SAGE_SESSION_H

#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

namespace ipme {
namespace wb {
class Sage_session : public std::enable_shared_from_this<Sage_session> {
public:
    using tcp = boost::asio::ip::tcp;

    explicit inline Sage_session(boost::asio::io_context& ioc)
        : resolver_{ioc}, ws_{ioc}
    {
    }

    void run(const std::string& host, const std::string& port,
             const std::string& init_message);

    std::string read();
    void write(const std::string& text);

    void close();

private:
    void on_resolve(boost::system::error_code ec,
                    tcp::resolver::results_type results);

    void on_connect(boost::system::error_code ec);

    void on_handshake(boost::system::error_code ec);

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_close(boost::system::error_code ec);

    tcp::resolver resolver_;
    boost::beast::websocket::stream<tcp::socket> ws_;
    boost::beast::multi_buffer buffer_;
    std::string host_;
    std::string init_message_;
};

} // namespace wb
} // namespace ipme

#endif // IPME_WB_SAGE_SESSION_H
