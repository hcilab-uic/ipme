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

#ifndef DEFAULT_SAGE_MESSAGE_HANDLER_H
#define DEFAULT_SAGE_MESSAGE_HANDLER_H

#include <string_view>

#include "sage_message_handler.h"
#include "utils/json.h"

namespace ipme::wb::sage {
class Default_sage_message_handler : public ipme::wb::Sage_message_handler {
public:
    inline Default_sage_message_handler(const std::string& name,
                                        const std::string& alias)
        : name_{name}, alias_{alias}
    {
    }

    inline std::string_view name() const override
    {
        return name_;
    }

    std::string_view alias() const override
    {
        return alias_;
    }

    void handle_message(std::istream& in) override;

    std::string generate_registration_message() override;

    void dispatch(const std::string& display_id,
                  const utils::Json& message) override;

    void set_element_container(
        std::shared_ptr<ipme::wb::sage::Sage_element_container> container)
        override;

protected:
    std::shared_ptr<ipme::wb::sage::Sage_element_container> container_;

private:
    std::string name_;
    std::string alias_;
    ipme::utils::Json json_;
};
} // namespace ipme::wb::sage

#endif // DEFAULT_SAGE_MESSAGE_HANDLER_H
