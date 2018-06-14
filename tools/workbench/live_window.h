#ifndef LIVE_WINDOW_H
#define LIVE_WINDOW_H

#include <QDialog>

namespace Ui {
class Live_window;
}

class Live_window : public QDialog
{
    Q_OBJECT

public:
    explicit Live_window(QWidget *parent = 0);
    ~Live_window();

private:
    Ui::Live_window *ui;
};

#endif // LIVE_WINDOW_H
