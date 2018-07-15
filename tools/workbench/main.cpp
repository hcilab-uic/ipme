#include "mainwindow.h"
#include <QApplication>

#include "utils/logger.h"
#include "workbench_window.h"

int main(int argc, char* argv[])
{
    // FIXME: This needs to be read from somewhere config and not hard-coded
    // like this
    std::string program_name{program_invocation_short_name};
    ipme::utils::Logger::init("log/" + program_name);
    ipme::utils::Logger::set_severity(boost::log::trivial::trace);

    QApplication app{argc, argv};

    Workbench_window workbench;
    workbench.show();

    return app.exec();
}
