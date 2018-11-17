#include "display.h"

#include <boost/asio/connect.hpp>
#include <boost/system/error_code.hpp>

namespace ipme::wb {
constexpr const char* add_client_msg_template =
    R"(
{
    "f":"0001",
    "d":
    {
        "clientType":"sageUI",
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

static const std::vector<std::string> subscribe_messages = {
    R"({"f":"0000","d":{"listener":"registerInteractionClient","alias":"0002"}})",
    R"({"f":"0000","d":{"listener":"getActiveClients","alias":"0003"}})",
    R"({"f":"0000","d":{"listener":"getRbac","alias":"0004"}})",
    R"({"f":"0000","d":{"listener":"updateAppState","alias":"0005"}})",
    R"({"f":"0000","d":{"listener":"updateStateOptions","alias":"0006"}})",
    R"({"f":"0000","d":{"listener":"appResize","alias":"0007"}})",
    R"({"f":"0000","d":{"listener":"appFullscreen","alias":"0008"}})",
    R"({"f":"0000","d":{"listener":"clearDisplay","alias":"0009"}})",
    R"({"f":"0000","d":{"listener":"createAppWindow","alias":"000a"}})",
    R"({"f":"0000","d":{"listener":"updateItemOrder","alias":"000b"}})",
    R"({"f":"0000","d":{"listener":"deleteAllApplications","alias":"000c"}})",
    R"({"f":"0000","d":{"listener":"tileApplications","alias":"000d"}})",
    R"({"f":"0000","d":{"listener":"addNewWebElement","alias":"000e"}})",
    R"({"f":"0000","d":{"listener":"openNewWebpage","alias":"000f"}})",
    R"({"f":"0000","d":{"listener":"startApplicationMove","alias":"0010"}})",
    R"({"f":"0000","d":{"listener":"startApplicationResize","alias":"0011"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationPosition","alias":"0012"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationPositionAndSize","alias":"0013"}})",
    R"({"f":"0000","d":{"listener":"finishApplicationMove","alias":"0014"}})",
    R"({"f":"0000","d":{"listener":"finishApplicationResize","alias":"0015"}})",
    R"({"f":"0000","d":{"listener":"deleteApplication","alias":"0016"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationState","alias":"0017"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationStateOptions","alias":"0018"}})",
    R"({"f":"0000","d":{"listener":"requestClientUpdate","alias":"0019"}})",
    R"({"f":"0000","d":{"listener":"setItemPosition","alias":"001a"}})",
    R"({"f":"0000","d":{"listener":"setItemPositionAndSize","alias":"001b"}})",
    R"({"f":"0000","d":{"listener":"deleteElement","alias":"001c"}})",
};

namespace detail {
void fail(boost::system::error_code ec, const char* what)
{
    ERROR() << what << ": " << ec.message() << "\n";
}
} // namespace detail

Display::Display(boost::asio::io_context& ioc, const std::string& name,
                 std::shared_ptr<data::Scene> __attribute__((unused)) scene)
    : resolver_{ioc}, ws_{ioc}, name_{name}
{
}

void Display::start(const std::string& host, const std::string& port)
{
    host_ = host;
    resolver_.async_resolve(host, port,
                            std::bind(&Display::on_resolve, shared_from_this(),
                                      std::placeholders::_1,
                                      std::placeholders::_2));
}

std::vector<std::shared_ptr<Display>>
Display::create_displays_from_scene(std::shared_ptr<data::Scene> scene)
{
}

void Display::read()
{
    ws_.async_read(read_buffer_,
                   std::bind(&Display::on_read, shared_from_this(),
                             std::placeholders::_1, std::placeholders::_2));
    TRACE() << "performed async_read()";
}

void Display::write(const std::string& message)
{
    ws_.async_write(boost::asio::buffer(message),
                    std::bind(&Display::on_write, shared_from_this(),
                              std::placeholders::_1, std::placeholders::_2));
    TRACE() << "performed sync_write()";
}

void Display::send_next_subscribe_message()
{
    if(subscribe_message_index_ < subscribe_messages.size()) {
        const std::string& msg = subscribe_messages[subscribe_message_index_];
        write(msg);
        ++subscribe_message_index_;
        DEBUG() << "TX: " << msg;
    } else if(subscribe_message_index_ == subscribe_messages.size()) {
        status_ = Status::active;
        read();
        DEBUG() << "set status to active, now reading";
    } else if(status_ == Status::active) {
        read();
        TRACE() << "status already active, nothing to send, attempting to read";
    }
}

void Display::on_resolve(boost::system::error_code __attribute__((unused)) ec,
                         tcp::resolver::results_type __attribute__((unused))
                         results)
{
    if(ec) {
        return detail::fail(ec, "resolve");
    }

    boost::asio::async_connect(ws_.next_layer(), results.begin(), results.end(),
                               std::bind(&Display::on_connect,
                                         shared_from_this(),
                                         std::placeholders::_1));
}

void Display::on_connect(boost::system::error_code ec)
{
    if(ec) {
        return detail::fail(ec, "connect");
    }

    ws_.async_handshake(host_, "/",
                        std::bind(&Display::on_handshake, shared_from_this(),
                                  std::placeholders::_1));
}

void Display::on_handshake(boost::system::error_code ec)
{
    if(ec) {
        return detail::fail(ec, "handshake");
    }

    write(add_client_msg_template);
}

void Display::on_write(boost::system::error_code __attribute__((unused)) ec,
                       std::size_t __attribute__((unused)) bytes_transferred)
{
    if(ec) {
        return detail::fail(ec, "write");
    }

    if(status_ == Status::initial) {
        INFO() << "[Session " << name_ << "] reading welcome messages";
        status_ = Status::receive_welcome_messages;
    } else if(status_ == Status::send_subscribe_messages) {
    } else if(status_ == Status::active) {
    }
}

void Display::on_read(boost::system::error_code __attribute__((unused)) ec,
                      std::size_t __attribute__((unused)) bytes_transferred)
{
    if(ec) {
        return detail::fail(ec, "read");
    }

    INFO() << "[session " << name_ << ":" << read_msg_count_
           << "] RX: " << boost::beast::buffers(read_buffer_.data());
    read_buffer_.consume(read_buffer_.size());

    read_msg_count_++;
    if(read_msg_count_ < init_message_count_) {
        TRACE() << "Still receiving initial messages, count "
                << read_msg_count_;
    } else if(read_msg_count_ == init_message_count_) {
        status_ = Status::send_subscribe_messages;
    }

    if(status_ == Status::receive_welcome_messages) {
        read();
    } else if(status_ == Status::send_subscribe_messages) {
        send_next_subscribe_message();
        TRACE() << "called send_next_subscribe_message()";
    } else if(status_ == Status::active) {
        read();
        TRACE() << "currently active and performing async reads()";
    }
}

void Display::on_close(boost::system::error_code __attribute__((unused)) ec)
{
    if(ec) {
        return detail::fail(ec, "close");
    }

    INFO() << "Connection closed by display " << name_;
}

} // namespace ipme::wb
