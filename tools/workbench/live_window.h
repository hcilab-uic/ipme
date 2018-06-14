#ifndef LIVE_WINDOW_H
#define LIVE_WINDOW_H

#include <QDialog>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class Live_window;
}

class Live_window : public QDialog {
    Q_OBJECT

public:
    explicit Live_window(QWidget* parent = 0);
    ~Live_window();

private slots:
    void on_start_experiment_button_clicked();

    void on_stop_experiment_button_clicked();

private:
    void process_video();

    bool run_experiment_ = false;
    Ui::Live_window* ui;
    cv::VideoCapture capture_;
    QTimer* capture_timer_;
};

#endif // LIVE_WINDOW_H
