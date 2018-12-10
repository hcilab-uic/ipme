#include "workbench_window.h"
#include "ui_workbench_window.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>

#include "config.h"
#include "live_window.h"
#include "mainwindow.h"
#include "utils/logger.h"
#include "visualization_window.h"

Workbench_window::Workbench_window(const ipme::wb::Config& config,
                                   int screen_index, QWidget* parent)
    : QMainWindow(parent),
      // clang-format off
      ui(new Ui::Workbench_window),
      live_window_{std::make_shared<Live_window>(config, this)},
      main_window_{std::make_shared<MainWindow>(this)},
      visualization_window_{std::make_shared<Visualization_window>(config, this)}
// clang-format on
{
    ui->setupUi(this);

    auto geometry = QApplication::screens()[screen_index]->availableGeometry();
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
                                    geometry));
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
    //    main_window_->show();
    visualization_window_->show();
}

void Workbench_window::on_button_launch_live_window_clicked()
{
    live_window_->show();
}
