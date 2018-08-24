#include "visualization_window.h"
#include "ui_visualization_window.h"

#include <QDir>
#include <QFileDialog>

Visualization_window::Visualization_window(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::Visualization_window)
{
    ui->setupUi(this);
}

Visualization_window::~Visualization_window()
{
    delete ui;
}

void Visualization_window::on_file_open_triggered()
{
    QString dir_path = QFileDialog::getExistingDirectory();
}
