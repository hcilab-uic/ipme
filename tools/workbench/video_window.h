#ifndef VIDEO_WINDOW_H
#define VIDEO_WINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

namespace Ui {
class Video_window;
}

class Video_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Video_window(QWidget *parent = 0);
    ~Video_window();

    void set_dirpath(const QString& dirpath);
private slots:
    void on_action_play_triggered();

    void on_action_pause_triggered();

    void on_action_stop_triggered();

private:
    void load_video();
    void process_video();


    bool play_video_ = false;
    Ui::Video_window *ui;
    QTimer* video_timer_{nullptr};
    QString dirpath_;
    cv::VideoCapture capture_;
};

#endif // VIDEO_WINDOW_H
