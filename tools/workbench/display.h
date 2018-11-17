#ifndef IPME_WB_DISPLAY_H
#define IPME_WB_DISPLAY_H

#include <memory>
#include <unordered_map>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "data/scene.h"

namespace ipme::wb {
class Display : std::enable_shared_from_this<Display> {
public:
    using tcp = boost::asio::ip::tcp;

    enum class Status
    {
        initial,
        receive_welcome_messages,
        send_subscribe_messages,
        active
    };

    Display(boost::asio::io_context& ioc, const std::string& name,
            std::shared_ptr<data::Scene> scene);

    void start(const std::string& host, const std::string& port);

    static std::vector<std::shared_ptr<Display>>
    create_displays_from_scene(std::shared_ptr<data::Scene> scene);

private:
    void read();

    void write(const std::string& message);

    void send_next_subscribe_message();

    void process_message(const std::string& message);

    // Handlers
    void on_resolve(boost::system::error_code ec,
                    tcp::resolver::results_type results);

    void on_connect(boost::system::error_code ec);

    void on_handshake(boost::system::error_code ec);

    void on_write(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_read(boost::system::error_code ec, std::size_t bytes_transferred);

    void on_close(boost::system::error_code ec);

    tcp::resolver resolver_;
    boost::beast::websocket::stream<tcp::socket> ws_;
    boost::beast::multi_buffer read_buffer_;

    Status status_ = Status::initial;
    std::size_t read_msg_count_ = 0;
    std::size_t subscribe_message_index_ = 0;
    std::string name_;
    std::string host_;

    static constexpr size_t init_message_count_ = 161;
};

} // namespace ipme::wb

#endif // DISPLAY_H
