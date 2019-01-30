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

#ifndef IPME_WB_SCENE_WIDGET_H
#define IPME_WB_SCENE_WIDGET_H

#include <vector>

#include <QOpenGLWidget>

#include "color.h"
#include "scene_object.h"

namespace ipme::wb {
class Scene_widget : public QOpenGLWidget {
public:
    Scene_widget(QWidget* widget);

    virtual void paintGL() override;

    inline void set_objects(const std::vector<Scene_object>& objects)
    {
        objects_ = objects;
    }

    void clear_objects();

    inline void set_colors(const std::vector<Color>& colors)
    {
        colors_ = colors;
    }

    inline void set_show_centerline(bool show)
    {
        show_centerline_ = show;
    }

    inline void set_show_head_ts(bool show)
    {
        show_head_ts_ = show;
    }

    inline void set_show_body_ts(bool show)
    {
        show_body_ts_ = show;
    }

    inline void set_show_device_ts(bool show)
    {
        show_device_ts_ = show;
    }

    inline void set_show_head_intersection(bool show)
    {
        show_head_intersection_ = show;
    }

    inline void set_show_body_intersection(bool show)
    {
        show_body_intersection_ = show;
    }

    inline void set_show_tight_coupling(bool show)
    {
        show_tight_coupling_ = show;
    }

    inline void set_show_loose_coupling(bool show)
    {
        show_loose_coupling_ = show;
    }

    /*!
     * \brief set_head_ts_angle
     * \param angle transactional segment for head
     */
    void set_head_ts_angle(float angle);

    /*!
     * \brief set_body_ts_angle
     * \param angle transactional segment for body
     */
    void set_body_ts_angle(float angle);

    /*!
     * \brief set_device_ts_angle
     * \param angle transactional segment for device
     */
    void set_device_ts_angle(float angle);

    /*!
     * \brief show_intersections
     */
    void show_intersections() const;

private:
    void show_intersections(size_t object_index1, size_t object_index2,
                            const Color& color) const;

    bool show_centerline_ = true;
    bool show_head_ts_ = false;
    bool show_body_ts_ = false;
    bool show_device_ts_ = false;
    bool show_head_intersection_ = false;
    bool show_body_intersection_ = false;
    bool show_tight_coupling_ = false;
    bool show_loose_coupling_ = false;

    std::vector<Scene_object> objects_;
    std::vector<Color> colors_{Color{.0f, .0f, .0f}};

    Scene_object::Ts_angles ts_angles_;
};
} // namespace ipme::wb

#endif // IPME_WB_SCENE_WIDGET_H
