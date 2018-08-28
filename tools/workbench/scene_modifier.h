#ifndef IPME_WB_SCENE_MODIFIER_H
#define IPME_WB_SCENE_MODIFIER_H

#include <vector>

#include <QObject>
#include <QVector3D>
#include <Qt3DCore/QEntity>

#include "data/scene.h"
#include "frame.h"
#include "protobuf/scene.pb.h"

namespace ipme::wb {
class Scene_modifier : public QObject {
    Q_OBJECT
public:
    explicit Scene_modifier(Qt3DCore::QEntity* root_entity);

    void add_frame(const Frame& frame);
    void add_screen();
    void clear();

    void set_screen_offset(const ipme::scene::Position& offset);

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
    void add_gaze(const ipme::scene::Pose& pose);

    Qt3DCore::QEntity* root_entity_;
    std::vector<Qt3DCore::QEntity*> spheres_;
    std::vector<Qt3DCore::QEntity*> entities_;
    QVector3D screen_offset_;
};

} // namespace ipme::wb

#endif // IPME_WB_SCENE_MODIFIER_H
