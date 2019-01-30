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

#ifndef IPME_WB_SAGE_SAGE_ELEMENT_H
#define IPME_WB_SAGE_SAGE_ELEMENT_H

#include <ostream>

#include "utils/json.h"

namespace ipme::wb::sage {
class Sage_element {
public:
    Sage_element(const std::string& id, const std::string& display_id,
                 double left, double top, double width, double height);

    void update(double left, double top, double width, double height);

    inline const std::string& id() const
    {
        return id_;
    }

    inline const std::string& display_id() const
    {
        return display_id_;
    }

    inline double left() const
    {
        return left_;
    }

    inline double top() const
    {
        return top_;
    }

    inline double width() const
    {
        return width_;
    }

    inline double height() const
    {
        return height_;
    }

    std::string to_string() const;

private:
    std::string id_;
    std::string display_id_;
    double left_ = 0.;
    double top_ = 0.;
    double width_ = 0.;
    double height_ = 0.;
};

std::ostream& operator<<(std::ostream& out, const Sage_element& element);
} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_SAGE_ELEMENT_H
