#include "live_window.h"
#include "ui_live_window.h"

Live_window::Live_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Live_window)
{
    ui->setupUi(this);
}

Live_window::~Live_window()
{
    delete ui;
}
