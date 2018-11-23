#ifndef LIVE_WINDOW_H
#define LIVE_WINDOW_H

#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <string_view>
#include <thread>

#include <QDialog>
#include <QStatusBar>
#include <QTime>
#include <QTimer>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "config.h"
#include "connector/omicronConnectorClient.h"
#include "data/scene.h"
#include "sage_handler.h"
#include "sensor/vicon_listener.h"
#include "sensor_manager.h"
#include "state_machine.h"

namespace Ui {
class Live_window;
}

class Live_window : public QDialog,
                    public ipme::wb::State_machine,
                    public std::enable_shared_from_this<Live_window> {
    Q_OBJECT

public:
    explicit Live_window(const ipme::wb::Config& config, QWidget* parent = 0);
    virtual ~Live_window() override;

    virtual State state() const override;
    virtual void set_state(const State state) override;

private slots:
    void on_start_experiment_button_clicked();

    void on_stop_experiment_button_clicked();

    void on_set_output_dir_button_clicked();

    void on_Live_window_destroyed();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void initialize_experiment();
    void start_experiment();
    void stop_experiment();

    void process_video();
    void process_vrpn();

    bool initialize_vrpn();
    bool initialize_camera();

    void init_file_setup();

    bool reset_camera();
    void stop_camera();

    void shutdown();

    void shutdown_vrpn();

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

    void add_new_frame();

    void set_status_indicator(QWidget* widget, bool status);

    Ui::Live_window* ui;
    ipme::wb::Config config_;
    cv::VideoCapture capture_;
    QTimer* capture_timer_;
    QString output_root_dir_;
    QStatusBar status_bar_;
    QTime elapsed_time_;
    //    std::string outfile_timestamp_;
    std::string video_filename_;
    cv::VideoWriter video_writer_;
    //    std::string timestamp_;
    std::filesystem::path output_dir_;
    //    std::string outfile_suffix_ = "0000";
    //    size_t outfile_index_ = 0;

    // needs to be mutable because binding to a shared_lock inside const
    // function violates constness
    mutable std::shared_mutex frame_number_mutex_;
    int frame_number_ = 0;

    mutable std::shared_mutex state_mutex_;
    ipme::wb::State_machine::State state_ =
        ipme::wb::State_machine::State::uninitialized;

    std::unique_ptr<std::thread> omicron_thread_;
    std::shared_ptr<ipme::data::Scene> scene_;
    //    ipme::wb::Sage_handler sage_handler_;
    //    ipme::sensor::Vicon_listener vrpn_listener_;
    ipme::wb::Sensor_manager display_manager_;

    std::shared_ptr<omicronConnector::OmicronConnectorClient> omicron_client_;
};

#endif // LIVE_WINDOW_H
