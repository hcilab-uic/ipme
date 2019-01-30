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

#ifndef IPME_WB_COLOR_H
#define IPME_WB_COLOR_H

#include <QString>

namespace ipme::wb {
struct Color {
    Color();

    /*!
     * \brief Color
     * \param r_
     * \param g_
     * \param b_
     * \param a_
     */
    Color(float r_, float g_, float b_, float a_ = 0.f);

    /// Return web hexadecimal format (eg: #FF0000 for (1.0, 0.0, 0.0))
    QString hex_string() const;

    QString stylesheet(const QString& fieldname) const;

    /// red
    float r = 0.f;

    /// green
    float g = 0.f;

    /// blue
    float b = 0.f;

    /// alpha
    float a = 0.f;
};
} // namespace ipme::wb

#endif // IPME_WB_COLOR_H
