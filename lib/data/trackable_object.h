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

#ifndef IPME_DATA_TRACKABLE_OBJECT_H
#define IPME_DATA_TRACKABLE_OBJECT_H

#include <memory>
#include <string>

namespace ipme {
namespace data {
class Trackable_object {
public:
    enum class Type : uint8_t
    {
        person,
        count
    };

    /**
     * Virtual destructor
     */
    virtual ~Trackable_object() = default;

    /**
     * @brief id accessor function for an alpha-numeric unique identifier
     * @return object identifier (eg: person_12)
     */
    virtual std::string id() const = 0;

    /**
     * @brief type object type
     * @return type
     */
    virtual Type type() const = 0;

    /**
     * @brief to_string string representation of the trackable object
     * @return stringified Trackable_objectj
     */
    virtual std::string to_string() const = 0;

    /**
     * @brief type_string convenience function to print trackable object type
     * @param type enum
     * @return string representation
     */
    static std::string type_string(const Type type);
};
} // namespace data
} // namespace ipme

#endif // IPME_DATA_TRACKABLE_OBJECT_H
