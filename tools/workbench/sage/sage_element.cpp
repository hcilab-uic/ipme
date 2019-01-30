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

#include "sage_element.h"

#include <iostream>
#include <sstream>

#include "utils/logger.h"

namespace ipme::wb::sage {
Sage_element::Sage_element(const std::string& id, const std::string& display_id,
                           double left, double top, double width, double height)
    : id_{id}, display_id_{display_id}
{
    update(left, top, width, height);
    INFO() << "CREATE [" << id << "]\n";
}

void Sage_element::update(double left, double top, double width, double height)
{
    // Some kind of unique/writer lock
    left_ = left;
    top_ = top;
    width_ = width;
    height_ = height;

    DEBUG() << "UPDATE [" << id_ << "] to L:" << left_ << " T: " << top_
            << " W:" << width_ << " H:" << height_;
}

std::string Sage_element::to_string() const
{
    std::stringstream ss;
    ss << "[" << id_ << "] to L:" << left_ << " T: " << top_ << " W:" << width_
       << " H:" << height_;
    return ss.str();
}

std::ostream& operator<<(std::ostream& out, const Sage_element& element)
{
    out << element.to_string();
    return out;
}

} // namespace ipme::wb::sage
