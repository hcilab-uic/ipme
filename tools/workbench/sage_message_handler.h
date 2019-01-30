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

#ifndef IPME_WB_SAGE_MESSAGE_HANDLER_H
#define IPME_WB_SAGE_MESSAGE_HANDLER_H

#include <istream>
#include <memory>
#include <string_view>

#include "sage/sage_element_container.h"
#include "utils/json.h"

namespace ipme {
namespace wb {
class Sage_message_handler {
public:
    using container_type = ipme::wb::sage::Sage_element_container;
    using container_ptr = std::shared_ptr<container_type>;

    virtual std::string_view name() const = 0;
    virtual std::string_view alias() const = 0;
    virtual void handle_message(std::istream& in) = 0;
    virtual std::string generate_registration_message() = 0;
    virtual void dispatch(const std::string& display_id,
                          const utils::Json& message) = 0;
    virtual void set_element_container(container_ptr container) = 0;
};
} // namespace wb
} // namespace ipme

#endif // IPME_WB_SAGE_MESSAGE_HANDLER_H
