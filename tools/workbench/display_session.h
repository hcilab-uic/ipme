#ifndef IPME_WB_DISPLAY_H
#define IPME_WB_DISPLAY_H

#include <memory>
#include <unordered_map>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "sage_message_handler.h"

#include "config.h"
#include "data/scene.h"

namespace ipme::wb {
class Display_session : public std::enable_shared_from_this<Display_session> {
public:
    using tcp = boost::asio::ip::tcp;
    using handler_ptr = std::shared_ptr<Sage_message_handler>;
    using handler_map_type = std::unordered_map<std::string, handler_ptr>;

    enum class Status
    {
        initial,
        receive_welcome_messages,
        send_subscribe_messages,
        active
    };

    Display_session(boost::asio::io_context& ioc,
                    std::shared_ptr<data::Scene> scene,
                    const Config::Sage_config& config,
                    const handler_map_type& handler_map);

    void start();

    void stop();

    static std::shared_ptr<Display_session>
    create(boost::asio::io_context& ioc, std::shared_ptr<data::Scene> scene,
           const Config::Sage_config& config,
           const handler_map_type& handler_map);

private:
    void read();

    void write(const std::string& message);

    void send_next_subscribe_message();

    void process_message(const std::string& message);

    void do_next();

    void fail(boost::system::error_code ec, const char* what);

    void set_status(Status status);

    void process_message();

    static std::string to_string(Status status);

    // Handlers
    void on_resolve(boost::system::error_code ec,
                    tcp::resolver::results_type results);

    void on_connect(boost::system::error_code ec);

    void on_handshake(boost::system::error_code ec);

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_close(boost::system::error_code ec);

    std::shared_ptr<data::Scene> scene_;
    tcp::resolver resolver_;
    boost::beast::websocket::stream<tcp::socket> ws_;
    boost::beast::multi_buffer read_buffer_;

    Status status_ = Status::initial;
    std::size_t read_count_ = 0;
    std::size_t subscribe_message_index_ = 0;
    std::string name_;
    Config::Sage_config config_;
    handler_map_type handler_map_;

    static constexpr size_t init_message_count_ = 161;
};

} // namespace ipme::wb

#endif // DISPLAY_H
