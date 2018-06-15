#ifndef LIVE_WINDOW_H
#define LIVE_WINDOW_H

#include <memory>
#include <shared_mutex>

#include <QDialog>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "connector/omicronConnectorClient.h"

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

    void on_pushButton_clicked();

    void on_set_output_dir_button_clicked();

private:
    void process_video();
    void initialize_vrpn();

    void update_frame_number(int frame_number);

    int frame_number() const;

    bool run_experiment_ = false;
    Ui::Live_window* ui;
    cv::VideoCapture capture_;
    QTimer* capture_timer_;
    QString output_dir_;

    // needs to be mutable because binding to a shared_lock inside const
    // function violates constness
    mutable std::shared_mutex frame_number_mutex_;
    int frame_number_ = 0;

    std::unique_ptr<omicronConnector::OmicronConnectorClient> omicron_client_;
};

#endif // LIVE_WINDOW_H
