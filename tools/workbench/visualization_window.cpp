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
#include "video_window.h"

#include "protobuf/scene.pb.h"

Visualization_window::Visualization_window(QWidget* parent)
    : QMainWindow{parent}, ui{new Ui::Visualization_window},
      root_entity_{new Qt3DCore::QEntity}, view_{new Qt3DExtras::Qt3DWindow},
      video_window_{std::make_unique<Video_window>(this)}
{
    ui->setupUi(this);

    ui->action_pitch_clockwise->setIcon(QIcon{":/icons/cw-x.png"});

    setWindowTitle("Visualization Window");
    make_axes();

    scene_modifier_ = std::make_unique<ipme::wb::Scene_modifier>(root_entity_);

    connect(this, &Visualization_window::current_frame_number, this,
            &Visualization_window::display_frame_number);

    auto& box = ui->vrpn_filter_policy_combobox;
    box->addItems(QStringList{"Average", "First", "Middle", "Last"});

    for(const auto& policy : ipme::wb::Frame::policy_map.left) {
        ui->frame_policy_combobox->addItem(policy.second.c_str());
    }

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

    video_window_->set_dirpath(dir_path);

    std::ifstream ifs{dir_path.toStdString() + "/tracking.pb"};

    ipme::scene::Scene scene_pb;
    scene_pb.ParseFromIstream(&ifs);

    const auto config = scene_pb.config();

    scene_modifier_->set_screen_offset(config.screen_offset());

    frames_.load(scene_pb);
    apply_frames_filter();
    ui->end_frame_edit->setText(QString::number(scene_pb.frames().size()));

    on_action_next_triggered();
}

void Visualization_window::show_frame(int frame_index)
{
    if(!frames_.exists(frame_index)) {
        return;
    }

    frame_index_ = frames_.get_frame_id(frame_index);
    show_current_frame();
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

void Visualization_window::apply_frames_filter()
{
    const auto filter_name = ui->frame_policy_combobox->currentText();
    frames_.apply_filter(filter_name.toStdString());
}

ipme::wb::Frame Visualization_window::filter(const ipme::wb::Frame& frame)
{
    if(ui->vrpn_filter_policy_combobox->currentText() != "First") {
        WARN() << "Only First is implemented";
    }

    return frame;
}

void Visualization_window::make_axes()
{
    //    auto x_axis = QQuaternion::fromAxisAndAngle(0, 0, 1, -90);
    //    make_axis(x_axis, 1.f, QRgb{0xff0000});
    //    auto y_axis = QQuaternion::fromAxisAndAngle(1, 0, 0, 0);
    //    make_axis(y_axis, 1.f, QRgb{0x00ff00});
    //    auto z_axis = QQuaternion::fromAxisAndAngle(1, 0, 0, 90);
    //    make_axis(z_axis, 1.f, QRgb{0x0000ff});
}

void Visualization_window::init()
{
    view_->defaultFrameGraph()->setClearColor(QColor{QRgb{0x4d4d4f}});
    auto container = QWidget::createWindowContainer(view_);
    auto screen_size = view_->screen()->size();
    container->setMinimumSize(QSize{200, 100});
    container->setMaximumSize(screen_size);
    auto widget = new QWidget;

    auto h_layout = new QHBoxLayout{widget};
    auto v_layout = new QVBoxLayout;

    v_layout->setAlignment(Qt::AlignTop);
    h_layout->addWidget(container, 1);
    h_layout->addLayout(v_layout);

    widget->setWindowTitle("Visualization Window");

    auto input = new Qt3DInput::QInputAspect;
    view_->registerAspect(input);
    auto camera_entity = view_->camera();
    camera_entity->lens()->setPerspectiveProjection(90.f, 16.f / 9.f, .1f,
                                                    1000.f);
    on_action_show_top_view_triggered();

    auto light_entity = new Qt3DCore::QEntity{root_entity_};
    auto light = new Qt3DRender::QPointLight{light_entity};
    light->setColor("white");
    light->setIntensity(1);
    light_entity->addComponent(light);

    auto light_transform = new Qt3DCore::QTransform{light_entity};
    light_transform->setTranslation(camera_entity->position());
    light_entity->addComponent(light_transform);

    auto controller = new Qt3DExtras::QOrbitCameraController{root_entity_};
    controller->setLinearSpeed(-50.f);
    controller->setLookSpeed(100.f);
    controller->setCamera(camera_entity);

    view_->setRootEntity(root_entity_);

    ui->scene_vertical_layout->addWidget(widget);
}

void Visualization_window::show_current_frame()
{
    if(frame_index_ > 0) {
        scene_modifier_->clear();
        scene_modifier_->add_screen();
        scene_modifier_->add_frame(frames_[frame_index_ - 1]);

        emit current_frame_number(frame_index_);
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

void Visualization_window::on_action_show_front_view_triggered()
{
    view_->camera()->setPosition(QVector3D{0, 0, 25.f});
    view_->camera()->setUpVector(QVector3D{0, 1, 0});
    view_->camera()->setViewCenter(QVector3D{0, 0, 0});
}

void Visualization_window::on_action_show_top_view_triggered()
{
    view_->camera()->setPosition(QVector3D{0, 25.f, 0});
    view_->camera()->setUpVector(QVector3D{0, 0, -1.f});
    view_->camera()->setViewCenter(QVector3D{0, 0, 0});
}

void Visualization_window::on_action_launch_video_triggered()
{
    video_window_->show();
    video_window_->set_scene_visualization(shared_from_this());
}

void Visualization_window::display_frame_number(int frame_number)
{
    const auto value = QString::number(frame_number);
    ui->frame_index_lcd->display(value);
    ui->end_frame_edit->setText(value);
}

void Visualization_window::on_save_outcome_button_clicked()
{
    if(labeled_file_path_.isEmpty()) {
        labeled_file_path_ = QFileDialog::getSaveFileName(
            this, tr("Export Labeled File"), "labeled.csv", "CSV file (*.csv)");
    }

    std::ofstream ofs{labeled_file_path_.toStdString(), std::ios::app};

    const auto record_vrpn_object = [&ofs](auto vrpn_object) {
        const auto pos = vrpn_object.position();
        const auto rot = vrpn_object.orientation();
        ofs << pos.x() << "," << pos.y() << "," << pos.z() << "," << rot.w()
            << "," << rot.x() << "," << rot.y() << "," << rot.z() << ",";
    };

    const int label = ui->outcome_label_edit->text().toInt();
    const size_t begin = ui->start_frame_edit->text().toULong();
    const size_t end = ui->end_frame_edit->text().toULong();
    for(size_t i = begin; i < end; ++i) {
        const auto frame = filter(frames_[i]);
        for(const auto& person : frame.persons) {
            record_vrpn_object(person);
        }

        for(const auto& device : frame.devices) {
            record_vrpn_object(device);
        }

        ofs << label << "\n";
    }

    ui->start_frame_edit->setText(QString::number(end));
}

void Visualization_window::on_frame_policy_combobox_currentIndexChanged(
    const QString& /*arg1*/)
{
    apply_frames_filter();
}
