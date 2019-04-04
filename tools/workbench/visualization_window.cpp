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

#include "visualization_window.h"
#include "ui_visualization_window.h"

#include <fstream>
#include <unordered_map>

#include <boost/tokenizer.hpp>

#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QQuaternion>
#include <QVBoxLayout>
#include <Qt3DCore>
#include <Qt3DExtras>
#include <Qt3DRender/QCamera>

#include "data/scene.h"
#include "protobuf/scene.pb.h"
#include "scene_modifier.h"
#include "similarity_finder.h"
#include "utils/json.h"
#include "utils/logger.h"
#include "utils/string_utils.h"
#include "utils/utils.h"
#include "video_window.h"

Visualization_window::Visualization_window(const ipme::wb::Config& config,
                                           QWidget* parent)
    : QMainWindow{parent}, ui{new Ui::Visualization_window},
      root_entity_{new Qt3DCore::QEntity},
      view_{std::make_shared<Qt3DExtras::Qt3DWindow>()},
      video_window_{std::make_shared<Video_window>(this)}, config_{config}
{
    ui->setupUi(this);

    setWindowTitle("Analysis");

    make_axes();

    scene_modifier_ = std::make_unique<ipme::wb::Scene_modifier>(root_entity_);

    connect(this, &Visualization_window::current_frame_number, this,
            &Visualization_window::display_frame_number);
    connect(this, &Visualization_window::replay_section, this,
            &Visualization_window::on_replay_section);
    connect(this, &Visualization_window::start_visualization,
            video_window_.get(), &Video_window::on_action_play_triggered);

    connect(this, &Visualization_window::set_video_frame_index,
            video_window_.get(), &Video_window::on_set_video_frame);

    connect(this, &Visualization_window::find_similar, this,
            &Visualization_window::on_find_similar);
    connect(this, &Visualization_window::show_log, this,
            &Visualization_window::on_show_log);

    auto& box = ui->vrpn_filter_policy_combobox;
    box->addItems(QStringList{"Average", "First", "Middle", "Last"});

    for(const auto& policy : ipme::wb::Frame::policy_map.left) {
        ui->frame_policy_combobox->addItem(policy.second.c_str());
    }

    init();
}

Visualization_window::~Visualization_window()
{
    if(root_entity_) {
        for(auto c : root_entity_->components()) {
            root_entity_->removeComponent(c);
            c = nullptr;
        }
        root_entity_ = nullptr;
    }

    delete ui;
    ui = nullptr;
}

void Visualization_window::on_file_open_triggered()
{
    const QString pbfile = "tracking.pb";
    QString dir_path =
        QFileDialog::getExistingDirectory(this, pbfile, QDir::homePath());
    DEBUG() << dir_path.toStdString() << "\n";

    video_window_->set_dirpath(dir_path);

    std::ifstream ifs{dir_path.toStdString() + "/" + pbfile.toStdString()};

    ipme::scene::Scene scene_pb;
    scene_pb.ParseFromIstream(&ifs);

    scene_modifier_->set_displays(scene_pb.config());

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
    auto container = QWidget::createWindowContainer(view_.get());
    auto screen_size = view_->screen()->size();
    container->setMinimumSize(QSize{200, 100});
    container->setMaximumSize(screen_size);
    auto widget = new QWidget;

    auto h_layout = new QHBoxLayout{widget};
    auto v_layout = new QVBoxLayout;

    v_layout->setAlignment(Qt::AlignTop);
    h_layout->addWidget(container, 1);
    h_layout->addLayout(v_layout);

    widget->setWindowTitle("Analysis & Visualization");

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

        int percent = static_cast<int>(static_cast<double>(frame_index_) * 100 /
                                       static_cast<double>(frames_.size()));

        ui->video_progressbar->setValue(percent);
        ui->progress_slider->setValue(percent);

        DEBUG() << "frame number " << frame_index_ << "/" << frames_.size()
                << " (" << percent << " percent)";
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
        std::string file_name{
            "labeled-" + ipme::utils::create_timestamp_string("%Y%m%d-%H%M%S") +
            "-.csv"};

        labeled_file_path_ =
            QFileDialog::getSaveFileName(this, tr("Export Labeled File"),
                                         file_name.c_str(), "CSV file (*.csv)");
    }

    std::ios::openmode mode =
        ui->overwrite_checkbox->isChecked() ? std::ios::out : std::ios::app;
    std::ofstream ofs{labeled_file_path_.toStdString(), mode};
    ofs << "seq_id,timestamp";

    for(int i = 0; i < config_.scene_config().registered_objects_size(); ++i) {
        const int index = i + 1;
        // clang-format off
        ofs << ",src_id" << index
            << ",x" << index
            << ",y" << index
            << ",z" << index
            << ",rw" << index
            << ",rx" << index
            << ",ry" << index
            << ",rz" << index;
        // clang-format on
    }
    ofs << ",label\n";

    const auto record_vrpn_object = [&ofs](auto vrpn_object) {
        const auto& pose = vrpn_object.pose();
        const auto pos = pose.position();
        const auto rot = pose.orientation();
        ofs << vrpn_object.vrpn_source_id() << "," << pos.x() << "," << pos.y()
            << "," << pos.z() << "," << rot.w() << "," << rot.x() << ","
            << rot.y() << "," << rot.z() << ",";
    };

    const auto selected_label =
        ui->outcome_label_combobox->currentText().toStdString();
    const int label = outcome_labels_[selected_label];

    const bool save_all_frames = ui->save_all_frames_checkbox->isChecked();
    const size_t requested_begin = ui->start_frame_edit->text().toULong();
    const size_t requested_end = ui->end_frame_edit->text().toULong();

    const size_t begin = save_all_frames ? 0 : requested_begin;
    const size_t end = save_all_frames
                           ? frames_.size()
                           : std::min(requested_end, frames_.size());

    size_t count_frames_saved{0};
    const bool validate_frame = ui->frame_validity_checkbox->isChecked();
    for(size_t i = begin; i < end; ++i) {
        //        const auto frame = filter(frames_[i]);
        const auto& frame = frames_[i];
        if(validate_frame && !frame.has_all_registered_ids()) {
            WARN() << "Frame " << i << " invalid, skipping";
            continue;
        }
        ofs << frame.frame_id() << "," << frame.timestamp() << ",";

        // std::map should not be used because it is slow. But I don't know
        // of another quick way to sort the keys. Until then, we shall use
        // this
        std::map<size_t, ipme::scene::Vrpn_object> map_to_sort;
        for(const auto& person : frame.persons) {
            //            record_vrpn_object(person);
            map_to_sort.emplace(person.vrpn_source_id(), person);
        }

        for(const auto& device : frame.devices) {
            //            record_vrpn_object(device);
            map_to_sort.emplace(device.vrpn_source_id(), device);
        }

        // Now take the sorted objects in the frame and write to ostream
        for(const auto& item : map_to_sort) {
            record_vrpn_object(item.second);
        }

        ofs << label << "\n";
        ++count_frames_saved;
    }

    ui->start_frame_edit->setText(QString::number(end));

    std::stringstream ss;
    ss << "Export complete, " << count_frames_saved << " frames saved";
    QMessageBox::information(this, tr("Data Export"), tr(ss.str().c_str()));
}

void Visualization_window::on_frame_policy_combobox_currentIndexChanged(
    const QString& /*arg1*/)
{
    apply_frames_filter();
}

void Visualization_window::on_action_load_labels_triggered()
{
    const auto label_config_file = QFileDialog::getOpenFileName(
        this, tr("Label config file"), "label.csv", "JSON file (*.csv)");
    std::ifstream ifs{label_config_file.toStdString()};
    std::string line;
    while(std::getline(ifs, line)) {
        const auto tokens = ipme::utils::split_string(line, ",");
        outcome_labels_.emplace(tokens[0], std::atoi(tokens[1].c_str()));
        ui->outcome_label_combobox->addItem(QString{tokens[0].c_str()});
    }
}

void Visualization_window::on_Visualization_window_destroyed()
{
    INFO() << "Shutting down visualization window";
}

void Visualization_window::on_replay_button_clicked()
{
    auto start = ui->start_frame_edit->text().toULong();
    auto end = ui->end_frame_edit->text().toULong();

    emit replay_section(start, end);
}

void Visualization_window::on_findsimilar_button_clicked()
{
    auto start = ui->start_frame_edit->text().toULong();
    auto end = ui->end_frame_edit->text().toULong();

    emit find_similar(start, end);
}

void Visualization_window::on_find_similar(size_t begin, size_t end)
{
    emit show_log("Finding similarity for ranges " + QString::number(begin) +
                  " to " + QString::number(end));
    similarity_finder_.find_similar(begin, end);

    auto similar_ranges = similarity_finder_.similar_ranges();
    std::stringstream ss;
    ss << "Found similarity in " << similar_ranges.size() << " intervals";
    INFO() << ss.str();
    emit show_log(ss.str().c_str());

    for(const auto& range : similar_ranges) {
        std::stringstream range_ss;
        range_ss << range.first << "-" << range.second;
        INFO() << range_ss.str();
        emit show_log(range_ss.str().c_str());
    }
}

void Visualization_window::on_action_start_viz_triggered()
{
    video_window_->show();
    video_window_->set_scene_visualization(shared_from_this());

    emit show_log("Playing video");
    emit video_window_->play_video();
}

void Visualization_window::on_replay_section(size_t begin, size_t end)
{
    emit show_log("replaying frames " + QString::number(begin) + " to " +
                  QString::number(end));
    video_window_->replay_section(begin, end);
}

void Visualization_window::on_action_stop_viz_triggered()
{
    emit show_log("Stopping video");
    emit video_window_->stop_video();
}

void Visualization_window::on_action_pause_viz_triggered()
{
    emit show_log("Pausing video");
    emit video_window_->pause_video();
}

void Visualization_window::on_progress_slider_sliderMoved(int position)
{
    DEBUG() << "slider moved to position " << position;
    frame_index_ = static_cast<size_t>(
        static_cast<double>(position * frames_.size()) / 100.0);
    ui->start_frame_edit->setText(QString::number(frame_index_));
    ui->end_frame_edit->setText(QString::number(frame_index_));

    emit set_video_frame_index(frame_index_);
}

void Visualization_window::on_show_log(const QString& msg)
{
    ui->log_window->append(msg);
}
