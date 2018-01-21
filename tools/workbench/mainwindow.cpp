#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <memory>

#include <QDebug>
#include <QDir>
#include <QErrorMessage>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

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

    connect(this, &MainWindow::show_log, this, &MainWindow::on_show_log);
    connect(this, &MainWindow::show_success_log, this,
            &MainWindow::on_show_success_log);
    connect(this, &MainWindow::show_warn_log, this,
            &MainWindow::on_show_warn_log);
    connect(this, &MainWindow::show_error_log, this,
            &MainWindow::on_show_error_log);

    ui->opengl_widget->set_show_tsegment(ui->tsegment_checkbox->isChecked());
    ui->opengl_widget->set_show_centerline(
        ui->centerline_checkbox->isChecked());

    emit show_log("Application started");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionFrame_Loader_triggered()
{
}

void MainWindow::on_clear_button_clicked()
{
    clear_scene();
}

void MainWindow::on_add_frame_button_clicked()
{
    //    auto frame_text = ui->frame_text->document()->toPlainText();
    //    load_table(frame_text);
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
        ui->object_count_lcd->display(QString{"%1"}.arg(objects.size()));
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

    //    emit show_log("Table loaded");
}

void MainWindow::show_row(int current_row)
{
    int col_count = ui->frame_table_widget->columnCount();
    QStringList row;
    for(int i = 0; i < col_count; ++i) {
        const auto item = ui->frame_table_widget->item(current_row, i);
        row.append(item->text());
    }

    show_scene(row);

    emit show_log("Showing current row " + QString::number(current_row));
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

void MainWindow::on_clear_text_button_clicked()
{
    //    ui->frame_text->clear();
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
    emit show_success_log("Number of CSV rows read: " + QString::number(row));
}

void MainWindow::on_clear_table_button_clicked()
{
    ui->frame_table_widget->clear();
    int rows = ui->frame_table_widget->rowCount();
    for(int i = 0; i < rows; ++i) {
        // Delete from bottom up, else index gets messed up
        ui->frame_table_widget->removeRow(rows - i);
    }
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
    ui->opengl_widget->set_show_centerline(arg1 != 0);
    ui->opengl_widget->update();
}

void MainWindow::on_tsegment_checkbox_stateChanged(int arg1)
{
    ui->opengl_widget->set_show_tsegment(arg1 != 0);
    ui->opengl_widget->update();
}

void MainWindow::on_actionExport_triggered()
{
    const auto selection =
        ui->frame_table_widget->selectionModel()->selectedRows();

    if(selection.count() == 0) {
        QString message{"No rows selected"};
        qDebug() << message;
        emit show_warn_log(message);
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

    emit show_log("Selection exported to file " + export_filename);
}

void MainWindow::on_show_log(const QString& message)
{
    ui->log_view_browser->insertPlainText(message + "\n");
}

void MainWindow::on_show_success_log(const QString& message)
{
    show_html_log(message, "green");
}

void MainWindow::on_show_warn_log(const QString& message)
{
    show_html_log(message, "#FFA500");
}

void MainWindow::on_show_error_log(const QString& message)
{
    show_html_log(message, "red");
}

void MainWindow::show_html_log(const QString& message, const QString& color)
{
    QString display_message =
        "<div style=\"color:" + color + "\";>" + message + "</div><br />";
    ui->log_view_browser->insertHtml(display_message);
}
