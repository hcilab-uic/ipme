#include "visualization_window.h"
#include "ui_visualization_window.h"

#include <fstream>
#include <unordered_map>

#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QQuaternion>
#include <QVBoxLayout>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender/QCamera>

#include <QLineEdit>

#include "data/scene.h"
#include "scene_modifier.h"
#include "utils/logger.h"

#include "protobuf/scene.pb.h"

Visualization_window::Visualization_window(QWidget* parent)
    : QMainWindow{parent}, ui{new Ui::Visualization_window},
      root_entity_{new Qt3DCore::QEntity}, view_{new Qt3DExtras::Qt3DWindow}
{
    ui->setupUi(this);
    setWindowTitle("Visualization Window");
    make_axes();
    init();
}

Visualization_window::~Visualization_window()
{
    delete ui;
}

void Visualization_window::on_file_open_triggered()
{
    QString dir_path = QFileDialog::getExistingDirectory(
        this, "tracking.pb", "/home/harish/ipme_experiments/study_20180810");
    DEBUG() << dir_path.toStdString() << "\n";

    std::ifstream ifs{dir_path.toStdString() + "/tracking.pb"};

    ipme::scene::Scene scene_pb;
    scene_pb.ParseFromIstream(&ifs);

    const auto config = scene_pb.config();
    std::unordered_map<uint32_t, std::string> registered_objects;
    for(const auto& object : config.registered_objects()) {
        registered_objects[object.id()] = object.name();
    }

    for(const auto& frame : scene_pb.frames()) {
        if(frame.screen_objects().size() == 0) {
            continue;
        }
        frames_.emplace_back(ipme::wb::Frame::create_from_pb(
            frame, config.screen_offset(), registered_objects));
    }
}
void Visualization_window::make_axis(float x, float y, float z, float length,
                                     const QColor& color)
{
    const QQuaternion rotation{1, x, y, z};
    make_axis(rotation, length, color);
}

void Visualization_window::make_axis(const QQuaternion& rotation, float length,
                                     const QColor& color)
{
    const float offset_x = -5.f;
    const float offset_y = -5.f;
    const float offset_z = 5.f;

    const QVector3D offset{offset_x, offset_y, offset_z};

    auto cylinder = new Qt3DExtras::QCylinderMesh;
    cylinder->setRadius(.005f);
    cylinder->setLength(length);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    auto cylinder_transform = new Qt3DCore::QTransform;
    cylinder_transform->setScale(length);
    cylinder_transform->setRotation(rotation);

    cylinder_transform->setTranslation(offset);

    auto cylinder_material = new Qt3DExtras::QPhongMaterial;
    cylinder_material->setDiffuse(color);

    auto cylinder_entity = new Qt3DCore::QEntity{root_entity_};
    cylinder_entity->addComponent(cylinder);
    cylinder_entity->addComponent(cylinder_material);
    cylinder_entity->addComponent(cylinder_transform);

    auto cone = new Qt3DExtras::QConeMesh;
    cone->setLength(.5f);
    cone->setTopRadius(.001f);
    cone->setBottomRadius(.15f);
    cone->setRings(50);
    cone->setSlices(20);

    auto cone_transform = new Qt3DCore::QTransform;
    cone_transform->setScale(1.f);
    const QVector3D cone_offset{offset_x, offset_y, offset_z};
    cone_transform->setTranslation(cone_offset);
    cone_transform->setRotation(rotation);

    auto cone_material = new Qt3DExtras::QPhongMaterial;
    cone_material->setDiffuse(color);

    auto cone_entity = new Qt3DCore::QEntity{root_entity_};
    cone_entity->addComponent(cone);
    cone_entity->addComponent(cone_material);
    cone_entity->addComponent(cone_transform);
}

void Visualization_window::make_axes()
{
    //    make_axis(1, 0, 0, 1.f, QRgb{0xff0000});
    //    make_axis(0, 1, 0, 1.f, QRgb{0x00ff00});
    //    make_axis(0, 0, 1, 1.f, QRgb{0x0000ff});
    //    QQuaternion x_axis{0, 1, 0, -3.14 / 2};
    auto x_axis = QQuaternion::fromAxisAndAngle(0, 0, 1, -90);
    make_axis(x_axis, 1.f, QRgb{0xff0000});
    auto y_axis = QQuaternion::fromAxisAndAngle(1, 0, 0, 0);
    make_axis(y_axis, 1.f, QRgb{0x00ff00});
    auto z_axis = QQuaternion::fromAxisAndAngle(1, 0, 0, 90);
    make_axis(z_axis, 1.f, QRgb{0x0000ff});
}

void Visualization_window::init()
{
    //    auto view_ = new Qt3DExtras::Qt3DWindow;
    view_->defaultFrameGraph()->setClearColor(QColor{QRgb{0x4d4d4f}});
    //    view->defaultFrameGraph()->setClearColor(QColor{QRgb{0x000000}});
    auto container = QWidget::createWindowContainer(view_);
    auto screen_size = view_->screen()->size();
    container->setMinimumSize(QSize{200, 100});
    //    container->setGeometry(0, 50, screen_size.width(),
    //    screen_size.height());
    container->setMaximumSize(screen_size);
    auto widget = new QWidget;
    //    QLineEdit* edit = new QLineEdit{widget};
    //    edit->setGeometry(0, 0, 100, 25);

    auto h_layout = new QHBoxLayout{widget};
    auto v_layout = new QVBoxLayout;

    v_layout->setAlignment(Qt::AlignTop);
    h_layout->addWidget(container, 1);
    h_layout->addLayout(v_layout);

    widget->setWindowTitle("Visualization Window");

    auto input = new Qt3DInput::QInputAspect;
    view_->registerAspect(input);
    //    auto root_entity = new Qt3DCore::QEntity;
    auto camera_entity = view_->camera();
    camera_entity->lens()->setPerspectiveProjection(90.f, 16.f / 9.f, .1f,
                                                    1000.f);
    camera_entity->setPosition(QVector3D{0, 0, 15.f});
    camera_entity->setUpVector(QVector3D{0, 1, 0});
    camera_entity->setViewCenter(QVector3D{0, 0, 0});

    auto light_entity = new Qt3DCore::QEntity{root_entity_};
    auto light = new Qt3DRender::QPointLight{light_entity};
    light->setColor("white");
    light->setIntensity(1);
    light_entity->addComponent(light);

    auto light_transform = new Qt3DCore::QTransform{light_entity};
    light_transform->setTranslation(camera_entity->position());
    light_entity->addComponent(light_transform);

    auto controller = new Qt3DExtras::QOrbitCameraController{root_entity_};
    controller->setLinearSpeed(-25.f);
    controller->setLookSpeed(100.f);
    controller->setCamera(camera_entity);

    scene_modifier_ = std::make_unique<ipme::wb::Scene_modifier>(root_entity_);

    view_->setRootEntity(root_entity_);

    setCentralWidget(widget);
}

void Visualization_window::show_current_frame()
{
    if(frame_index_ > 0) {
        scene_modifier_->clear();
        scene_modifier_->add_frame(frames_[frame_index_ - 1]);
    }
}

void Visualization_window::on_action_next_triggered()
{
    if(frame_index_ < static_cast<int64_t>(frames_.size())) {
        ++frame_index_;
    }

    show_current_frame();
}

void Visualization_window::on_action_previous_triggered()
{
    if(frame_index_ > 0) {
        --frame_index_;
    }

    show_current_frame();
}
