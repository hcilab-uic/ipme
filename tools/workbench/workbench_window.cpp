#include "workbench_window.h"
#include "ui_workbench_window.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>

#include "live_window.h"
#include "mainwindow.h"

Workbench_window::Workbench_window(QWidget* parent)
    : QMainWindow(parent),
      // clang-format off
      ui(new Ui::Workbench_window),
      live_window_{std::make_shared<Live_window>(this)},
      main_window_{std::make_shared<MainWindow>(this)}
// clang-format on
{
    ui->setupUi(this);
    setGeometry(
        QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
                            QApplication::desktop()->availableGeometry()));
}

Workbench_window::~Workbench_window()
{
    if(ui) {
        delete ui;
        ui = nullptr;
    }
}

void Workbench_window::on_button_launch_analysis_window_clicked()
{
    main_window_->show();
}

void Workbench_window::on_button_launch_live_window_clicked()
{
    live_window_->show();
}
