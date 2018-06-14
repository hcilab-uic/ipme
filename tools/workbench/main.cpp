#include "mainwindow.h"
#include <QApplication>

#include "workbench_window.h"

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};

    Workbench_window workbench;
    workbench.show();

    return app.exec();
}
