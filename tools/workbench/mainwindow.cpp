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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <memory>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QErrorMessage>
#include <QFile>
#include <QFileDialog>
#include <QScrollBar>
#include <QTextStream>
#include <QTimer>

#include "color.h"
#include "scene_object.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), colors_{{1.f, 0.f, 0.f},
                                                           {0.f, 1.f, 0.f},
                                                           {0.f, 0.f, 1.f},
                                                           {1.f, 1.f, 0.f},
                                                           {1.f, 0.f, 1.f}}
{
    ui->setupUi(this);
    ui->opengl_widget->set_colors(colors_);

    connect(this, &MainWindow::log, this, &MainWindow::on_log);
    connect(this, &MainWindow::success, this, &MainWindow::on_success);
    connect(this, &MainWindow::warn, this, &MainWindow::on_warn);
    connect(this, &MainWindow::error, this, &MainWindow::on_error);

    ui->opengl_widget->set_show_head_ts(ui->head_ts_checkbox->isChecked());
    ui->opengl_widget->set_show_body_ts(ui->body_ts_checkbox->isChecked());
    ui->opengl_widget->set_show_device_ts(ui->device_ts_checkbox->isChecked());

    ui->opengl_widget->set_head_ts_angle(
        ui->head_ts_angle_lineedit->text().toFloat());
    ui->opengl_widget->set_body_ts_angle(
        ui->body_ts_angle_lineedit->text().toFloat());
    ui->opengl_widget->set_device_ts_angle(
        ui->device_ts_angle_lineedit->text().toFloat());

    ui->opengl_widget->set_show_centerline(
        ui->centerline_checkbox->isChecked());

    video_timer_ = new QTimer{this};
    connect(video_timer_, &QTimer::timeout, this, &MainWindow::process_video);
    video_timer_->start(1000 / 24);

    emit log("Application started");
}

MainWindow::~MainWindow()
{
    if(video_timer_) {
        delete video_timer_;
        video_timer_ = nullptr;
    }

    delete ui;
}

void MainWindow::on_frame_table_widget_itemClicked(QTableWidgetItem* item)
{
    int current_row = item->row();
    show_row(current_row);
}

void MainWindow::on_frame_table_widget_itemDoubleClicked(QTableWidgetItem* item)
{
    ui->frame_table_widget->removeRow(item->row());
}

void MainWindow::show_scene(const QString& frame_text)
{
    const auto objects = ipme::wb::Scene_object::create_from_csv(frame_text);
    show_scene(objects);
}

void MainWindow::show_scene(const QStringList& frame_cells)
{
    const auto objects =
        ipme::wb::Scene_object::create_from_cells(frame_cells, 1);
    show_scene(objects);
}

void MainWindow::show_scene(const std::vector<ipme::wb::Scene_object>& objects)
{
    // Setup legend
    ui->head_label->setStyleSheet(colors_[0].stylesheet("background"));
    ui->solar_plexus_label->setStyleSheet(colors_[1].stylesheet("background") +
                                          "; color: black;");
    ui->right_arm_label->setStyleSheet(colors_[2].stylesheet("background"));
    ui->left_arm_label->setStyleSheet(colors_[3].stylesheet("background") +
                                      "; color: black;");
    ui->device_label->setStyleSheet(colors_[4].stylesheet("background") +
                                    "; color: black;");

    // Display scene
    if(objects.size() > 0) {
        ui->timestamp_lcd->display(objects[0].timestamp());
    }

    ui->opengl_widget->set_objects(objects);
    ui->opengl_widget->update();
}

void MainWindow::load_table(const QString& line)
{
    auto text_cells = line.split(",");

    const int row = ui->frame_table_widget->rowCount();
    ui->frame_table_widget->setRowCount(row + 1);
    ui->frame_table_widget->setColumnCount(text_cells.size());

    for(int i = 0; i < text_cells.size(); ++i) {
        auto cell_content = new QTableWidgetItem(tr("%1").arg(text_cells[i]));
        ui->frame_table_widget->setItem(row, i, cell_content);
    }
}

bool MainWindow::show_row(int current_row)
{
    const int total_items = ui->frame_table_widget->rowCount();
    if(total_items == 0) {
        emit error("No table loaded, nothing to show");
        return false;
    }

    int last_valid_row = total_items - 1;
    if(current_row > last_valid_row || current_row < 0) {
        emit error("Invalid row value, enter a value between 0 and " +
                   QString::number(last_valid_row));
        return false;
    }

    int col_count = ui->frame_table_widget->columnCount();
    QStringList row;
    for(int i = 0; i < col_count; ++i) {
        const auto item = ui->frame_table_widget->item(current_row, i);
        row.append(item->text());
    }

    show_scene(row);
    show_value_message("Showing frame", current_row, "magenta");

    return true;
}

void MainWindow::clear_scene()
{
    // clear scene
    ui->opengl_widget->clear_objects();

    // Clear legends
    ui->head_label->setStyleSheet("background: white;");
    ui->solar_plexus_label->setStyleSheet("background: white;");
    ui->right_arm_label->setStyleSheet("background: white;");
    ui->left_arm_label->setStyleSheet("background: white;");
    ui->device_label->setStyleSheet("background: white;");
}

void MainWindow::on_actionOpen_triggered()
{
    const auto file_name = QFileDialog::getOpenFileName(
        this, tr("Open CSV file"), QDir::homePath(),
        tr("Capture Files (*.csv)"));

    qInfo() << file_name;
    QFile file{file_name};

    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        QString msg{"Could not open file " + file_name + ", " +
                    file.errorString()};
        qCritical() << msg;
        QErrorMessage error{this};
        error.showMessage(msg);

        return;
    }

    QTextStream in{&file};

    int row{0};
    while(!in.atEnd()) {
        QString line = in.readLine();
        ++row;
        if(row == 1) {
            continue;
        }
        load_table(line);
    }

    //    qInfo() << "Number of CSV rows read: " << row;
    emit success("Number of CSV rows read: " + QString::number(row));
}

void MainWindow::on_next_frame_button_clicked()
{
    if(ui->frame_table_widget->rowCount() == 0) {
        return;
    }

    const auto items = ui->frame_table_widget->selectedItems();
    const int current_row = items.size() == 0 ? -1 : items.first()->row();
    if(current_row < ui->frame_table_widget->rowCount()) {
        int next_row = current_row + 1;
        ui->frame_table_widget->selectRow(next_row);
        show_row(next_row);
    }
}

void MainWindow::on_previous_frame_button_clicked()
{
    if(ui->frame_table_widget->rowCount() == 0) {
        return;
    }

    const auto items = ui->frame_table_widget->selectedItems();
    const int current_row = items.size() == 0
                                ? ui->frame_table_widget->rowCount()
                                : items.last()->row();
    if(current_row > 0) {
        int previous_row = current_row - 1;
        ui->frame_table_widget->selectRow(previous_row);
        show_row(previous_row);
    }
}

void MainWindow::on_centerline_checkbox_stateChanged(int arg1)
{
    bool show_center_line = arg1 != 0;
    ui->opengl_widget->set_show_centerline(show_center_line);
    ui->opengl_widget->update();

    show_value_message("Showing center line", show_center_line);
}

void MainWindow::on_head_ts_checkbox_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_head_ts(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing head transaction segment", show_tsegment);
}

void MainWindow::on_body_ts_checkbox_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_body_ts(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing body transaction segment", show_tsegment);
}

void MainWindow::on_device_ts_checkbox_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_device_ts(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing device transaction segment", show_tsegment);
}

void MainWindow::on_log(const QString& message)
{
    //    ui->log_view_browser->insertPlainText(message + "\n");
    show_html_log(message, "cyan");
}

void MainWindow::on_success(const QString& message)
{
    show_html_log(message, "green");
}

void MainWindow::on_warn(const QString& message)
{
    show_html_log(message, "#FFA500");
}

void MainWindow::on_error(const QString& message)
{
    show_html_log(message, "red");
}

void MainWindow::show_html_log(const QString& message, const QString& color)
{
    auto time = "[" + QTime::currentTime().toString("hh:mm.ss.zzz") + "] ";
    QString display_message = time + "<span style=\"color:" + color + "\";>" +
                              message + "</span><br />";
    ui->log_view_browser->insertHtml(display_message);

    auto scroll_bar = ui->log_view_browser->verticalScrollBar();
    scroll_bar->setValue(scroll_bar->maximum());
}

void MainWindow::on_action_load_video_triggered()
{
    const QString filters{"AVI files (*.avi);;MP4 files (*.mp4);;MOV files "
                          "(*.mov);;All Files(*.*)"};
    const auto file_name = QFileDialog::getOpenFileName(
        this, tr("Open CSV file"), QDir::homePath(), filters);

    capture_.open(file_name.toStdString());
    video_total_frames_ =
        static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_COUNT));
    last_video_frame_ = video_total_frames_ - 1;

    int frame_width = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture_.get(CV_CAP_PROP_FRAME_HEIGHT));
    ui->video_label->resize(frame_width, frame_height);

    emit success("Opened video file " + file_name +
                 ", frames loaded: " + QString::number(video_total_frames_) +
                 ", frame width: " + QString::number(frame_width) +
                 ", height: " + QString::number(frame_height) +
                 ", FPS: " + QString::number(capture_.get(CV_CAP_PROP_FPS)));
}

void MainWindow::process_video()
{
    if(!capture_.isOpened()) {
        return;
    }

    if(capture_.isOpened() && video_play_) {
        cv::Mat frame;
        capture_ >> frame;

        if(frame.empty()) {
            return;
        }

        cv::cvtColor(frame, frame, CV_BGR2RGB);
        cv::resize(frame, frame, cv::Size(640, 360), 0, 0, cv::INTER_CUBIC);
        QImage widget_image(static_cast<uchar*>(frame.data), frame.cols,
                            frame.rows, frame.step, QImage::Format_RGB888);

        ui->video_label->setPixmap(QPixmap::fromImage(widget_image));

        int frame_number =
            static_cast<int>(capture_.get(CV_CAP_PROP_POS_FRAMES));
        ui->video_frame_number->display(frame_number);

        if(video_total_frames_ > 0) {
            int progress = static_cast<int>(
                static_cast<double>(frame_number) /
                static_cast<double>(video_total_frames_) * 100.0);
            ui->video_slider->setValue(progress);
        }

        if(frame_number == last_video_frame_) {
            video_play_ = false;
            capture_.set(CV_CAP_PROP_POS_FRAMES, 0);
            ui->playpause_button->setText("Play");
            ui->video_slider->setValue(0);

            emit log("Reached the end of video");
        }
    }
}

QString MainWindow::to_string(bool value) const
{
    return value ? "true" : "false";
}

void MainWindow::on_playpause_button_clicked()
{
    auto text = ui->playpause_button->text();
    video_play_ = text == "Play";
    ui->playpause_button->setText(video_play_ ? "Pause" : "Play");

    if(!video_play_) {
        emit log("Paused at frame " +
                 QString::number(capture_.get(CV_CAP_PROP_POS_FRAMES)) +
                 " at " +
                 QString::number(capture_.get(CV_CAP_PROP_POS_MSEC) / 1000) +
                 " seconds");
    }
}

void MainWindow::on_video_slider_sliderMoved(int position)
{
    double frame_to_advance = static_cast<double>(position) / 100.00 *
                              static_cast<double>(video_total_frames_);
    capture_.set(CV_CAP_PROP_POS_FRAMES, frame_to_advance);
    emit log("Slider moved to position " + QString::number(position) +
             ", jumping to frame " +
             QString::number(static_cast<int>(frame_to_advance)) + ", " +
             QString::number(capture_.get(CV_CAP_PROP_POS_MSEC) / 1000) +
             " seconds");
}

void MainWindow::on_visualized_frame_selectionChanged()
{
    ui->visualized_frame->clear();
}

void MainWindow::on_visualized_frame_returnPressed()
{
    int frame_number = ui->visualized_frame->text().toInt();
    if(show_row(frame_number)) {
        emit show_value_message("Showing requested frame", frame_number);
    }
}

void MainWindow::on_show_intersection_checkbox_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_head_intersection(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing intersecting head transactional segments",
                       show_tsegment);
}

void MainWindow::on_body_intersection_checkbox_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_body_intersection(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing intersecting body transactional segments",
                       show_tsegment);
}

void MainWindow::on_tight_coupling_box_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_tight_coupling(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing tight couplings in transactional segments",
                       show_tsegment);
}

void MainWindow::on_loose_coupling_box_stateChanged(int arg1)
{
    bool show_tsegment = arg1 != 0;

    ui->opengl_widget->set_show_loose_coupling(show_tsegment);
    ui->opengl_widget->update();

    show_value_message("Showing loose couplings in transactional segments",
                       show_tsegment);
}

void MainWindow::on_actionExport_triggered()
{
    const auto selection =
        ui->frame_table_widget->selectionModel()->selectedRows();

    if(selection.count() == 0) {
        QString message{"No rows selected"};
        qDebug() << message;
        emit warn(message);
        return;
    }

    std::vector<QString> rows;
    const auto column_count = ui->frame_table_widget->columnCount();
    for(const auto& row : selection) {
        QString row_text;
        int row_id = row.row();
        for(int col = 0; col < column_count; ++col) {
            row_text += ui->frame_table_widget->item(row_id, col)->text();
            if(col < column_count - 1) {
                row_text += ",";
            }
        }

        rows.push_back(row_text);
    }

    QFileDialog dialog;
    const auto export_filename =
        dialog.getSaveFileName(this, "File to export", "", "*.csv");

    std::ofstream output_file{export_filename.toStdString().c_str()};
    for(const auto& row : rows) {
        output_file << row.toStdString() << "\n";
    }

    emit log("Selection exported to file " + export_filename);
}

void MainWindow::on_actionAppend_to_file_triggered()
{
    if(append_file_.isEmpty()) {
        QFileDialog dialog;
        append_file_ =
            dialog.getSaveFileName(this, "File to append", "", "*.csv");
    }

    const auto column_count = ui->frame_table_widget->columnCount();
    const auto selection =
        ui->frame_table_widget->selectionModel()->selectedRows();
    std::ofstream outfile{append_file_.toStdString().c_str(), std::ios::app};

    for(const auto& row : selection) {
        QString row_text;
        int row_id = row.row();
        for(int col = 0; col < column_count; ++col) {
            row_text += ui->frame_table_widget->item(row_id, col)->text();
            if(col < column_count - 1) {
                row_text += ",";
            }
        }
        row_text += "\n";
        emit log("Appending to file: " + row_text);
        outfile << row_text.toStdString();
    }
}

void MainWindow::on_head_ts_angle_lineedit_returnPressed()
{
    on_head_ts_angle_lineedit_editingFinished();
}

void MainWindow::on_head_ts_angle_lineedit_editingFinished()
{
    float angle = ui->head_ts_angle_lineedit->text().toFloat();

    if(angle < 0.f || angle > 180.f) {
        emit warn("Please enter an angle between 0 and 180 degrees");
        return;
    }

    ui->opengl_widget->set_head_ts_angle(angle);
    emit success("Head transaction segment angle changed to " +
                 QString::number(angle) + " degrees");
}

void MainWindow::on_head_ts_angle_lineedit_selectionChanged()
{
    ui->head_ts_angle_lineedit->clear();
}

void MainWindow::on_device_ts_angle_lineedit_returnPressed()
{
    on_device_ts_angle_lineedit_editingFinished();
}

void MainWindow::on_device_ts_angle_lineedit_editingFinished()
{
    float angle = ui->device_ts_angle_lineedit->text().toFloat();

    if(angle < 0.f || angle > 180.f) {
        emit warn("Please enter an angle between 0 and 180 degrees");
        return;
    }

    ui->opengl_widget->set_device_ts_angle(angle);
    emit success("Device transaction segment angle changed to " +
                 QString::number(angle) + " degrees");
}

void MainWindow::on_device_ts_angle_lineedit_selectionChanged()
{
    ui->device_ts_angle_lineedit->clear();
}

void MainWindow::on_body_ts_angle_lineedit_returnPressed()
{
    on_body_ts_angle_lineedit_editingFinished();
}

void MainWindow::on_body_ts_angle_lineedit_editingFinished()
{
    float angle = ui->body_ts_angle_lineedit->text().toFloat();

    if(angle < 0.f || angle > 180.f) {
        emit warn("Please enter an angle between 0 and 180 degrees");
        return;
    }

    ui->opengl_widget->set_body_ts_angle(angle);
    emit success("Body transaction segment angle changed to " +
                 QString::number(angle) + " degrees");
}

void MainWindow::on_body_ts_angle_lineedit_selectionChanged()
{
    ui->body_ts_angle_lineedit->clear();
}

void MainWindow::on_clear_all_clicked()
{
    const int row_count = ui->frame_table_widget->rowCount();
    for(int i = row_count - 1; i >= 0; --i) {
        ui->frame_table_widget->removeRow(i);
    }
    clear_scene();
}
