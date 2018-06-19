#ifndef LIVE_WINDOW_H
#define LIVE_WINDOW_H

#include <memory>
#include <shared_mutex>
#include <string_view>

#include <QDialog>
#include <QStatusBar>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "connector/omicronConnectorClient.h"
#include "sage_handler.h"

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

    void on_set_output_dir_button_clicked();

private:
    void process_video();
    bool initialize_vrpn();
    bool initialize_camera();
    bool initialize_sage();

    bool reset_camera();
    void stop_camera();

    void set_start_button_state(std::string_view text, std::string_view color);
    inline void set_start_button_start()
    {
        set_start_button_state("Start", "green");
    }

    inline void set_start_button_pause()
    {
        set_start_button_state("Pause", "yellow");
    }

    inline void set_start_button_init()
    {
        set_start_button_state("Init", "blue");
    }

    void set_status(std::string_view status,
                    std::string_view color = "#68a2ff");

    void show_message(const QString& message);

    void enable_stop_button();
    void disable_stop_button();

    void update_frame_number(int frame_number);

    int frame_number() const;

    void set_status_indicator(QWidget* widget, bool status);

    Ui::Live_window* ui;
    cv::VideoCapture capture_;
    QTimer* capture_timer_;
    QString output_dir_;
    QStatusBar status_bar_;

    // needs to be mutable because binding to a shared_lock inside const
    // function violates constness
    mutable std::shared_mutex frame_number_mutex_;
    int frame_number_ = 0;

    ipme::wb::Sage_handler sage_handler_;

    enum class experiment_state
    {
        uninitialized,
        initialized,
        running,
        paused,
        stopped
    };
    experiment_state current_state_ = experiment_state::uninitialized;

    std::unique_ptr<omicronConnector::OmicronConnectorClient> omicron_client_;
};

#endif // LIVE_WINDOW_H
