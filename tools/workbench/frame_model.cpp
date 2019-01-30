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

#include "frame_model.h"

namespace ipme::wb {
Frame_model::Frame_model()
{
}

QVariant Frame_model::data(const QModelIndex& index, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant{};
    }

    const size_t r = static_cast<size_t>(index.row());
    const size_t c = static_cast<size_t>(index.column());

    if(r < data_.size() && c < data_[0].size()) {
        return data_[r][c];
    }

    return QVariant{};
}
} // namespace ipme::wb
