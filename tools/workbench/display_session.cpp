/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "display_session.h"

#include <sstream>

#include <boost/asio/connect.hpp>
#include <boost/system/error_code.hpp>

#include "utils/json.h"

#define SID "[session " << name_ << "] [status:" << to_string(status_) << "] "

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
    R"({"f":"0000","d":{"listener":"deleteApplication", "alias":"0016"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationState","alias":"0017"}})",
    R"({"f":"0000","d":{"listener":"updateApplicationStateOptions","alias":"0018"}})",
    R"({"f":"0000","d":{"listener":"requestClientUpdate","alias":"0019"}})",
    R"({"f":"0000","d":{"listener":"setItemPosition","alias":"001a"}})",
    R"({"f":"0000","d":{"listener":"setItemPositionAndSize","alias":"001b"}})",
    R"({"f":"0000","d":{"listener":"deleteElement","alias":"001c"}})",
};

Display_session::Display_session(boost::asio::io_context& ioc,
                                 std::shared_ptr<data::Scene> scene,
                                 const Config::Sage_config& config,
                                 const handler_map_type& handler_map)
    // clang-format off
    : scene_{scene}
    , resolver_{ioc}
    , ws_{ioc}
    , name_{config.id}
    , config_{config}
    , handler_map_{handler_map}
// clang-format on
{
}

bool Display_session::initialize()
{
    resolver_.async_resolve(
        config_.host.c_str(), std::to_string(config_.port).c_str(),
        std::bind(&Display_session::on_resolve, shared_from_this(),
                  std::placeholders::_1, std::placeholders::_2));
    TRACE() << SID << "called async_resolve(" << config_.host << ":"
            << config_.port << ")";
    return true;
}

void Display_session::on_resolve(boost::system::error_code ec,
                                 tcp::resolver::results_type results)
{
    if(ec) {
        return fail(ec, "resolve");
    }

    TRACE() << SID << "in on_resolve() with " << results.size() << " results";
    boost::asio::async_connect(ws_.next_layer(), results.begin(), results.end(),
                               std::bind(&Display_session::on_connect,
                                         shared_from_this(),
                                         std::placeholders::_1));
    TRACE() << SID << "called async_connect()";
}

void Display_session::on_connect(boost::system::error_code ec)
{
    if(ec) {
        return fail(ec, "connect");
    }

    TRACE() << SID << "in on_connect()";
    ws_.async_handshake(config_.host, "/",
                        std::bind(&Display_session::on_handshake,
                                  shared_from_this(), std::placeholders::_1));
    TRACE() << SID << "called async_handshake()";
}

void Display_session::on_handshake(boost::system::error_code ec)
{
    if(ec) {
        return fail(ec, "handshake");
    }

    TRACE() << SID << "in on_handshake()";
    write(add_client_msg_template);
    TRACE() << SID << "called write() initial message";
}

void Display_session::on_write(boost::system::error_code ec,
                               std::size_t bytes_transferred)
{
    if(ec) {
        return fail(ec, "write");
    }

    TRACE() << SID << "wrote " << bytes_transferred << " bytes";

    if(status_ == Status::initial) {
        set_status(Status::receive_welcome_messages);
        INFO() << SID << "session status to receive initial messages";
        read();
    } else if(status_ == Status::send_subscribe_messages) {
        send_next_subscribe_message();
        DEBUG() << SID << "sent next subscribe message";
    } else if(status_ == Status::active) {
        read();
        DEBUG() << SID << "called read()";
    }
}

void Display_session::on_read(boost::system::error_code ec,
                              std::size_t bytes_transferred)
{
    if(ec) {
        return fail(ec, "read");
    }

    if(status_ == Status::active) {
        process_message();
        read();
        return;
    }

    INFO() << SID << "RX: " << boost::beast::buffers(read_buffer_.data());
    TRACE() << SID << "read " << bytes_transferred << " bytes on the last read";

    read_count_++;
    if(read_count_ < init_message_count_) {
        TRACE() << SID << read_count_
                << " initial messages received, current status "
                << to_string(status_);
    } else if(read_count_ == init_message_count_) {
        INFO() << SID
               << "initial messages complete, preparing to send subscribe "
                  "messages";
        set_status(Status::send_subscribe_messages);
    }

    if(status_ == Status::receive_welcome_messages) {
        read();
    } else if(status_ == Status::send_subscribe_messages) {
        send_next_subscribe_message();
        TRACE() << SID << "called send_next_subscribe_message()";
    }

    read_buffer_.consume(read_buffer_.size());
}

void Display_session::on_close(boost::system::error_code ec)
{
    TRACE() << SID << "Handling close on display " << name_;

    if(ec) {
        return fail(ec, "close");
    }

    INFO() << SID << "Connection to display " << name_ << " closed";
}

void Display_session::read()
{
    TRACE() << SID << "reading with async_read()";
    ws_.async_read(read_buffer_,
                   std::bind(&Display_session::on_read, shared_from_this(),
                             std::placeholders::_1, std::placeholders::_2));
}

void Display_session::write(const std::string& message)
{
    ws_.async_write(boost::asio::buffer(message),
                    std::bind(&Display_session::on_write, shared_from_this(),
                              std::placeholders::_1, std::placeholders::_2));
    TRACE() << SID << "async_write(): " << message;
}

void Display_session::stop()
{
    ws_.async_close(boost::beast::websocket::close_code::normal,
                    std::bind(&Display_session::on_close, shared_from_this(),
                              std::placeholders::_1));
    INFO() << SID << "stop called";
}

std::shared_ptr<Display_session>
Display_session::create(boost::asio::io_context& ioc,
                        std::shared_ptr<data::Scene> scene,
                        const Config::Sage_config& config,
                        const Display_session::handler_map_type& handler_map)
{
    return std::make_shared<Display_session>(ioc, scene, config, handler_map);
}

void Display_session::send_next_subscribe_message()
{
    if(subscribe_message_index_ < subscribe_messages.size()) {
        const std::string msg = subscribe_messages[subscribe_message_index_];
        write(msg);
        ++subscribe_message_index_;
        DEBUG() << SID << "TX: " << msg;
    } else if(subscribe_message_index_ == subscribe_messages.size()) {
        status_ = Status::active;
        read();
        DEBUG() << SID << "set status to active, now reading";
    } else if(status_ == Status::active) {
        read();
        TRACE() << SID << "status active, nothing to send, attempting to read";
    }
}

void Display_session::do_next()
{
}

void Display_session::fail(boost::system::error_code ec, const char* what)
{
    ERROR() << SID << what << ": " << ec.message();
}

/* static */ std::string
Display_session::to_string(Display_session::Status status)
{
    switch(status) {
    case Status::initial:
        return "initial";

    case Status::receive_welcome_messages:
        return "receive_welcome_messages";

    case Status::send_subscribe_messages:
        return "send_subscribe_messages";

    case Status::active:
        return "active";

    default:
        return "<invalid-status>";
    }
}

void Display_session::set_status(Display_session::Status status)
{
    DEBUG() << "Setting status to " << to_string(status);
    status_ = status;
}

void Display_session::process_message()
{
    std::stringstream ss;
    ss << boost::beast::buffers(read_buffer_.data());
    read_buffer_.consume(read_buffer_.size());

    try {
        utils::Json json;
        json.read(ss);
        const std::string handler_name = json.get("f");
        auto handler = handler_map_.find(handler_name);
        if(handler == std::end(handler_map_)) {
            ERROR() << SID << "no handler for message type " << handler_name;
            return;
        }
        handler->second->dispatch(name_, json);
        DEBUG() << SID << "handled message " << ss.str();
    } catch(const std::exception&) {
        WARN() << SID << "could not proces " << ss.str();
    }
}

} // namespace ipme::wb
