#ifndef WORKBENCH_WINDOW_H
#define WORKBENCH_WINDOW_H

#include <memory>

#include <QMainWindow>

#include "config.h"
#include "live_window.h"
#include "mainwindow.h"

namespace Ui {
class Workbench_window;
}

class Workbench_window : public QMainWindow {
    Q_OBJECT

public:
    explicit Workbench_window(const ipme::wb::Config& config, int screen_index,
                              QWidget* parent = 0);
    ~Workbench_window();

private slots:
    void on_button_launch_analysis_window_clicked();

    void on_button_launch_live_window_clicked();

private:
    Ui::Workbench_window* ui;
    std::shared_ptr<Live_window> live_window_;
    std::shared_ptr<MainWindow> main_window_;
    cv::VideoCapture capture_;
};

#endif // WORKBENCH_WINDOW_H
