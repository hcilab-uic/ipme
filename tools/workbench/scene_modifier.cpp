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
        add_gaze(person);
    }

    for(const auto& device : frame.devices) {
        add_cuboid(device, QVector3D{1.f, 1.f, 0.05f}, device_color);
        add_gaze(device);
    }

    for(const auto& screen_object : frame.screen_objects) {
        QVector3D extents{screen_object.width * scale_factor,
                          screen_object.height * scale_factor, 0.05f};
        add_cuboid(screen_object.pose, extents, ui_color);
    }
}

void Scene_modifier::add_screen()
{
    //    static constexpr float x_extent = 7.36f;
    //    static constexpr float y_extent = 2.4f;
    //    static constexpr float z_extent = 0.001f;

    //    auto cuboid = new Qt3DExtras::QCuboidMesh;
    //    cuboid->setXExtent(x_extent * scale_factor);
    //    cuboid->setYExtent(y_extent * scale_factor);
    //    cuboid->setZExtent(z_extent);

    //    auto cuboid_transform = new Qt3DCore::QTransform;

    //    QVector3D offset{(screen_offset_.x() + (x_extent / 2)) * scale_factor,
    //                     (screen_offset_.y() + (y_extent / 2)) * scale_factor,
    //                     screen_offset_.z() * scale_factor};
    //    cuboid_transform->setTranslation(offset);
    //    cuboid_transform->setScale(1.f);

    //    auto cuboid_material = new Qt3DExtras::QPhongMaterial;
    //    cuboid_material->setDiffuse(0xffffff);

    //    entities_.emplace_back(new Qt3DCore::QEntity{root_entity_});
    //    auto& cuboid_entity = entities_.back();

    //    cuboid_entity->addComponent(cuboid);
    //    cuboid_entity->addComponent(cuboid_material);
    //    cuboid_entity->addComponent(cuboid_transform);
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

void Scene_modifier::set_screen_offset(const ipme::scene::Position& offset)
{
    screen_offset_.setX(static_cast<float>(offset.x()));
    screen_offset_.setY(static_cast<float>(offset.y()));
    screen_offset_.setZ(static_cast<float>(offset.z()));
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

void Scene_modifier::add_gaze(const ipme::scene::Pose& pose)
{
    constexpr float length = 2.f;
    auto cylinder = new Qt3DExtras::QCylinderMesh;
    cylinder->setRadius(0.03f);
    cylinder->setLength(length);

    auto cylinder_transform = new Qt3DCore::QTransform;
    cylinder_transform->setScale(1.f);

    auto rotation = make_rotation(pose.orientation());
    auto offset = rotation.rotatedVector(QVector3D{0, length / 2, 0});

    auto translation = make_position_vector(pose.position());
    auto translation_with_offset =
        QVector3D{translation.x() + offset.x(), translation.y() + offset.y(),
                  translation.z() + offset.z()};
    cylinder_transform->setTranslation(translation_with_offset);
    cylinder_transform->setRotation(rotation);

    auto cylinder_material = new Qt3DExtras::QPhongMaterial;
    cylinder_material->setDiffuse(QColor{QRgb{0xffffff}});

    entities_.emplace_back(new Qt3DCore::QEntity{root_entity_});
    auto& cylinder_entity = entities_.back();

    cylinder_entity->addComponent(cylinder);
    cylinder_entity->addComponent(cylinder_material);
    cylinder_entity->addComponent(cylinder_transform);
}

} // namespace ipme::wb
