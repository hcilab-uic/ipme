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

#ifndef IPME_WB_SAGE_HANDLER_H
#define IPME_WB_SAGE_HANDLER_H

#include <cstdlib>
#include <memory>
#include <string_view>
#include <thread>
#include <unordered_map>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "data/scene.h"
#include "sage/sage_element_container.h"
#include "sage_message_handler.h"
#include "sage_session.h"
#include "state_machine.h"

namespace ipme {
namespace wb {
class Sage_handler {
public:
    Sage_handler(std::shared_ptr<data::Scene> scene);
    ~Sage_handler();

    bool connect(std::string_view host, std::string_view port,
                 std::string_view session_token);
    void disconnect();

    bool start();

    void stop();

    void set_state_machine(std::shared_ptr<State_machine> state_machine);

    void flush();

private:
    void init_handlers();
    void internal_start();

    void apply_transform(const sage::Sage_element& element);

    //    boost::asio::io_context ioc_;
    //    boost::asio::ip::tcp::resolver resolver_;
    //    boost::beast::websocket::stream<boost::asio::ip::tcp::socket>
    //    wstream_;
    std::unique_ptr<std::thread> sage_thread_;
    std::shared_ptr<State_machine> state_machine_;
    std::unordered_map<std::string, std::shared_ptr<Sage_message_handler>>
        handler_map_;
    std::shared_ptr<ipme::wb::sage::Sage_element_container> element_container_;
    std::shared_ptr<data::Scene> scene_;

    //    std::shared_ptr<Sage_session> session_;
    //    boost::asio::io_context async_ioc_;
};

} // namespace wb
} // namespace ipme

#endif // IPME_WB_SAGE_HANDLER_H
