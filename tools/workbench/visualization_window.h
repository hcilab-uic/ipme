#ifndef VISUALIZATION_WINDOW_H
#define VISUALIZATION_WINDOW_H

#include <QMainWindow>

namespace Ui {
class Visualization_window;
}

class Visualization_window : public QMainWindow {
    Q_OBJECT

public:
    explicit Visualization_window(QWidget* parent = 0);
    ~Visualization_window();

private slots:
    void on_file_open_triggered();

private:
    Ui::Visualization_window* ui;
};

#endif // VISUALIZATION_WINDOW_H
