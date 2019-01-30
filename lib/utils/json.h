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

#ifndef IPME_UTILS_JSON_H
#define IPME_UTILS_JSON_H

#include <filesystem>
#include <iostream>
#include <istream>
#include <ostream>
#include <string_view>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace ipme {
namespace utils {
class Json {
public:
    void read(const std::filesystem::path& path);

    void read(std::istream& in);

    template <typename ElementValue = std::string>
    ElementValue get(std::string_view path) const;

    boost::property_tree::ptree get_node(std::string_view path)
    {
        return ptree_.get_child(path.data());
    }

    std::string to_string() const;

    inline const boost::property_tree::ptree& root() const
    {
        return ptree_;
    }

private:
    boost::property_tree::ptree ptree_;
};
} // namespace utils
} // namespace ipme

#include "json.ipp"

#endif // IPME_UTILS_JSON_H
