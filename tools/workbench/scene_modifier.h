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

#ifndef IPME_WB_SCENE_MODIFIER_H
#define IPME_WB_SCENE_MODIFIER_H

#include <vector>

#include <QObject>
#include <QVector2D>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include "data/scene.h"
#include "frame.h"
#include "protobuf/scene.pb.h"

namespace ipme::wb {
class Scene_modifier : public QObject {
    Q_OBJECT
public:
    struct Scene_display {
        std::string display_id;
        QVector3D offset;
        QVector2D dimensions;
    };

    explicit Scene_modifier(Qt3DCore::QEntity* root_entity);

    void add_frame(const Frame& frame);
    void add_screen();
    void clear();

    //    void set_screen_offset(const ipme::scene::Position& offset);
    void set_displays(const ipme::scene::Scene_config& config);

private:
    static const QColor default_color;
    static const QColor person_color;
    static const QColor device_color;
    static const QColor center_color;
    static const QColor ui_color;

    void add_sphere(const ipme::scene::Pose& pose,
                    const QColor& color = default_color);

    void add_cuboid(const ipme::scene::Pose& pose, QVector3D extents,
                    const QColor& color);
    void add_looking_direction(const ipme::scene::Pose& pose);

    Qt3DCore::QEntity* root_entity_;

    using entity_container_type = std::vector<Qt3DCore::QEntity*>;
    using entity_container_ptr = std::shared_ptr<entity_container_type>;

    entity_container_ptr active_entities_;
    entity_container_ptr inactive_entities_;

    std::vector<Scene_display> displays_;
};

} // namespace ipme::wb

#endif // IPME_WB_SCENE_MODIFIER_H
