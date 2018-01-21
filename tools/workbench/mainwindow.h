#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>

#include "color.h"
#include "scene_object.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

signals:
    void show_log(const QString& message);
    void show_warn_log(const QString& message);
    void show_success_log(const QString& message);
    void show_error_log(const QString& message);

private slots:
    void on_actionFrame_Loader_triggered();

    void on_clear_button_clicked();

    //    void on_plot_button_clicked();

    void on_add_frame_button_clicked();

    //    void on_remove_frame_button_clicked();

    void on_frame_table_widget_itemClicked(QTableWidgetItem* item);

    void on_frame_table_widget_itemDoubleClicked(QTableWidgetItem* item);

    void on_clear_text_button_clicked();

    void on_actionOpen_triggered();

    void on_clear_table_button_clicked();

    void on_next_frame_button_clicked();

    void on_previous_frame_button_clicked();

    void on_centerline_checkbox_stateChanged(int arg1);

    void on_tsegment_checkbox_stateChanged(int arg1);

    void on_actionExport_triggered();

    void on_show_log(const QString& message);
    void on_show_success_log(const QString& message);
    void on_show_warn_log(const QString& message);
    void on_show_error_log(const QString& message);

private:
    void show_html_log(const QString& message, const QString& color);

    void show_scene(const QString& frame_text);
    void show_scene(const QStringList& frame_cells);
    void show_scene(const std::vector<ipme::wb::Scene_object>& objects);

    void load_table(const QString& line);

    void show_row(int current_row);

    void clear_scene();

    Ui::MainWindow* ui;
    const std::vector<ipme::wb::Color> colors_;
};

#endif // MAINWINDOW_H
