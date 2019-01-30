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

#ifndef IPME_WB_SCENE_OBJECT_H
#define IPME_WB_SCENE_OBJECT_H

#include <array>
#include <vector>

#include <QString>

#include <boost/polygon/polygon.hpp>

#include "color.h"
#include "core/polygon.h"
#include "geometry.h"

namespace ipme::wb {
class Scene_object {
    static constexpr size_t point_dimension = 3;
    static constexpr size_t vector_dimenstion = 3;
    static constexpr size_t cells_per_object = 7;

public:
    struct Ts_angles {
        float head = pi / 6;
        float body = pi / 6;
        float device = pi / 4;
    };

    Scene_object(const std::vector<QString>& parameters,
                 const QString& timestamp = "not-set");
    Scene_object(const std::vector<float>& parameters,
                 const QString& timestamp);

    virtual ~Scene_object() = default;

    static std::vector<Scene_object>
    create_from_csv(const QString& line, int ignore_cell_count = 2,
                    const QString& delimiter = ",");
    static std::vector<Scene_object>
    create_from_cells(const QStringList cells, int ignore_cell_count = 2);

    inline const std::array<float, point_dimension>& coords() const
    {
        return coords_;
    }

    inline const std::array<float, vector_dimenstion>& rot_vector() const
    {
        return rot_vector_;
    }

    inline float angle() const
    {
        return angle_;
    }

    inline const QString& timestamp() const
    {
        return timestamp_;
    }

    core::Polygon3f transaction_segment() const;

    virtual void draw(bool show_centerline, bool show_head_ts,
                      bool show_body_ts, bool show_device_ts,
                      const Ts_angles& ts_angles);

    void draw_transaction_segment() const;

    void set_ts_angle(float angle);

    void set_reverse_transaction_segment();

private:
    core::Polygon3f::ring_type
    create_polygon_ring(core::Polygon3f::point_type::coordinate_type x,
                        core::Polygon3f::point_type::coordinate_type y,
                        core::Polygon3f::point_type::coordinate_type z,
                        float center_angle) const;

    static size_t next_index();

    size_t object_index_;
    Color color_;
    std::array<float, point_dimension> coords_;
    std::array<float, vector_dimenstion> rot_vector_;
    float ts_angle_ = pi / 6.f;
    float angle_;
    QString timestamp_;

    static const std::vector<Color> object_colors;
    static constexpr float max_length = 2.f;
};

} // namespace ipme::wb

#endif // IPME_WB_SCENE_OBJECT_H
