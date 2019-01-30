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

#ifndef IPME_WB_FRAME_MODEL_H
#define IPME_WB_FRAME_MODEL_H

#include <QAbstractItemModel>

namespace ipme::wb {
class Frame_model : public QAbstractItemModel {
public:
    Frame_model();

    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const override;

private:
    std::vector<std::vector<float>> data_{{10.f, 20.f}, {20.f, 30.f}};
};
} // namespace ipme::wb

#endif // IPME_WB_FRAME_MODEL_H
