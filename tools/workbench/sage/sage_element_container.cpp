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

#include "sage_element_container.h"

#include "utils/logger.h"

namespace ipme::wb::sage {
void Sage_element_container::add_element(const std::string& display_id,
                                         const ipme::utils::Json& message)
{
    auto id = message.get("d.id");
    auto left = message.get<double>("d.left");
    auto top = message.get<double>("d.top");
    auto width = message.get<double>("d.width");
    auto height = message.get<double>("d.height");

    add_element(id, display_id, left, top, width, height);
}

void Sage_element_container::add_element(const std::string& id,
                                         const std::string& display_id,
                                         double left, double top, double width,
                                         double height)
{
    auto itr = elements_.emplace(
        id, Sage_element{id, display_id, left, top, width, height});
    INFO() << "Added element " << itr.second;
}

void Sage_element_container::update_element(const std::string& id,
                                            const std::string& display_id,
                                            double left, double top,
                                            double width, double height)
{
    auto itr = elements_.find(id.data());
    if(itr != std::end(elements_)) {
        itr->second.update(left, top, width, height);
        DEBUG() << "Updated element " << itr->second;
    } else {
        INFO() << "Update received from an unknown element " << id << " adding";
        add_element(id, display_id, left, top, width, height);
    }
}

void Sage_element_container::delete_element(const std::string& id)
{
    auto itr = elements_.find(id);
    if(itr != std::end(elements_)) {
        elements_.erase(itr);
        INFO() << "[DELETE] " << itr->second;
    }
}
} // namespace ipme::wb::sage
