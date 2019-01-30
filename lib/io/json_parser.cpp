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

#include "json_parser.h"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace ipme {
namespace io {
Json_parser::Json_parser(const boost::filesystem::path& filename)
    : path_{filename}
{
}

boost::property_tree::ptree Json_parser::parse()
{
    boost::property_tree::json_parser::read_json(path_.string(), pt_);
    //    boost::property_tree::write_json(std::cout, pt_, true);
    return pt_;
}

} // namespace io
} // namespace ipme
