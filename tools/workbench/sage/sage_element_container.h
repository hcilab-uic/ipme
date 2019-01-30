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

#ifndef IPME_WB_SAGE_ELEMENT_CONTAINER_H
#define IPME_WB_SAGE_ELEMENT_CONTAINER_H

#include <string_view>
#include <unordered_map>

#include "sage_element.h"
#include "utils/json.h"

namespace ipme::wb::sage {
class Sage_element_container {
public:
    using Container_type = std::unordered_map<std::string, Sage_element>;

    void add_element(const std::string& display_id,
                     const ipme::utils::Json& message);
    void add_element(const std::string& id, const std::string& display_id,
                     double left, double top, double width, double height);
    void update_element(const std::string& id, const std::string& display_id,
                        double left, double top, double width, double height);
    void delete_element(const std::string& id);

    inline const Container_type& elements() const
    {
        // FIXME: Some kind of shared/reader lock on the entire container here
        return elements_;
    }

private:
    // This is monotonically increasing, does not decrease on delete
    size_t element_add_counter_ = 0;

    Container_type elements_;
};

} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_ELEMENT_CONTAINER_H
