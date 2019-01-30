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

#ifndef IPME_IO_JSON_PARSER_H
#define IPME_IO_JSON_PARSER_H

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include "parser.h"

namespace ipme {
namespace io {
using ptree = boost::property_tree::ptree;

class Json_parser : public Parser {
public:
    /**
     * @brief Json_parser constructor taking path of the file to be parsed
     * @param filename name of the file
     */
    Json_parser(const boost::filesystem::path& filename);

    /** @copydoc Parser::parse()
     */
    virtual boost::property_tree::ptree parse() override;

private:
    ptree pt_;
    boost::filesystem::path path_;
};

} // namespace io
} // namespace ipme
#endif // IPME_IO_JSON_PARSER_H
