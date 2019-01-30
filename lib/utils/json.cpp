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

#include "json.h"

#include <sstream>

namespace ipme {
namespace utils {
namespace pt = boost::property_tree;

void Json::read(const std::filesystem::__cxx11::path& path)
{
    std::ifstream in{path};
    read(in);
}

void Json::read(std::istream& in)
{
    pt::read_json(in, ptree_);
}

std::string Json::to_string() const
{
    std::ostringstream ss;
    pt::write_json(ss, ptree_);
    return ss.str();
}

} // namespace utils
} // namespace ipme
