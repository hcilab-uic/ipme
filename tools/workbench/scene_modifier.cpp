#include "scene_modifier.h"

#include <QQuaternion>
#include <Qt3DCore>
#include <Qt3DExtras>

namespace ipme::wb {
constexpr float scale_factor = 5.f;

const QColor Scene_modifier::default_color{QRgb{0xa69929}};
const QColor Scene_modifier::person_color = default_color;
const QColor Scene_modifier::device_color{QRgb{0xa699ff}};
const QColor Scene_modifier::center_color{QRgb{0xff0000}};
const QColor Scene_modifier::ui_color{QRgb{0xff6600}};

QVector3D make_position_vector(const ipme::scene::Position& pos)
{
    return QVector3D{static_cast<float>(pos.x()) * scale_factor,
                     static_cast<float>(pos.y()) * scale_factor,
                     static_cast<float>(pos.z()) * scale_factor};
}

QQuaternion make_rotation(const ipme::scene::Quaternion& rot)
{
    return QQuaternion{static_cast<float>(rot.w()), static_cast<float>(rot.x()),
                       static_cast<float>(rot.y()),
                       static_cast<float>(rot.z())};
}

Scene_modifier::Scene_modifier(Qt3DCore::QEntity* root_entity)
    : root_entity_{root_entity}
{
}

void Scene_modifier::add_frame(const Frame& frame)
{
    //    add_sphere(0, 0, 0, center_color);
    for(const auto& person : frame.persons) {
        add_sphere(person);
    }

    for(const auto& device : frame.devices) {
        add_cuboid(device, QVector3D{1.f, 1.f, 0.05f}, device_color);
    }

    for(const auto& screen_object : frame.screen_objects) {
        QVector3D extents{screen_object.width, screen_object.width, 0.05f};
        add_cuboid(screen_object.pose, extents, ui_color);
    }
}

void Scene_modifier::clear()
{
    for(auto& entity : spheres_) {
        for(auto& component : entity->components()) {
            entity->removeComponent(component);
            delete component;
            component = nullptr;
        }

        delete entity;
        entity = nullptr;
    }

    spheres_.clear();

    for(auto& entity : entities_) {
        for(auto& component : entity->components()) {
            entity->removeComponent(component);
            delete component;
            component = nullptr;
        }

        delete entity;
        entity = nullptr;
    }
    entities_.clear();
}

void Scene_modifier::add_sphere(const ipme::scene::Pose& pose,
                                const QColor& color)
{
    auto sphere_transform = new Qt3DCore::QTransform;
    sphere_transform->setScale(1.f);
    sphere_transform->setTranslation(make_position_vector(pose.position()));

    auto sphere = new Qt3DCore::QEntity{root_entity_};
    sphere->addComponent(sphere_transform);

    auto sphere_mesh = new Qt3DExtras::QSphereMesh;
    sphere_mesh->setRadius(.5f);

    sphere->addComponent(sphere_mesh);

    auto sphere_material = new Qt3DExtras::QPhongMaterial;
    sphere_material->setDiffuse(color);

    sphere->addComponent(sphere_material);

    sphere->setEnabled(true);
    spheres_.push_back(sphere);
}

void Scene_modifier::add_cuboid(const ipme::scene::Pose& pose,
                                QVector3D extents, const QColor& color)
{
    auto cuboid = new Qt3DExtras::QCuboidMesh;
    cuboid->setXExtent(extents.x());
    cuboid->setZExtent(extents.y());
    cuboid->setYExtent(extents.z());

    auto cuboid_transform = new Qt3DCore::QTransform;
    cuboid_transform->setScale(1.f);
    cuboid_transform->setTranslation(make_position_vector(pose.position()));
    cuboid_transform->setRotation(make_rotation(pose.orientation()));

    auto cuboid_material = new Qt3DExtras::QPhongMaterial;
    cuboid_material->setDiffuse(color);

    entities_.emplace_back(new Qt3DCore::QEntity{root_entity_});
    auto& cuboid_entity = entities_.back();

    cuboid_entity->addComponent(cuboid);
    cuboid_entity->addComponent(cuboid_material);
    cuboid_entity->addComponent(cuboid_transform);
}

} // namespace ipme::wb
