#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <type_traits>

#include <QDateTime>
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

    void on_action_load_video_triggered();

    void on_playpause_button_clicked();

    void on_video_slider_sliderMoved(int position);

    void on_visualized_frame_selectionChanged();

    void on_visualized_frame_returnPressed();

    void on_show_intersection_checkbox_stateChanged(int arg1);

private:
    void show_html_log(const QString& message, const QString& color);

    void show_scene(const QString& frame_text);
    void show_scene(const QStringList& frame_cells);
    void show_scene(const std::vector<ipme::wb::Scene_object>& objects);

    void load_table(const QString& line);

    bool show_row(int current_row);

    void clear_scene();

    void process_video();

    template <typename Value>
    void show_value_message(const QString& message, Value&& value,
                            const QString& color = "cyan")
    {
        QString display{
            "<span style=\"font-style: italic; font-weight: bold;\">" +
            to_string(std::forward<Value>(value)) + QString{"</span>"}};
        show_html_log(message + ": " + display, color);
    }

    template <typename Value>
    QString to_string(Value&& value) const
    {
        //        static_assert(std::is_arithmetic<Value>(value),
        //                      "value is not a number");

        return QString::number(value);
    }

    QString to_string(bool value) const;

    Ui::MainWindow* ui;
    const std::vector<ipme::wb::Color> colors_;

    int video_total_frames_{0};
    int last_video_frame_{0};

    QDateTime time_keeper_;
    cv::VideoCapture capture_;
    bool video_play_ = false;
    QTimer* video_timer_{nullptr};
};

#endif // MAINWINDOW_H
