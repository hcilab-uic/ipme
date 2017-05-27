#include <boost/program_options.hpp>
#include <iostream>
#include <mutex>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>

#include "sensor/sensor.h"
#include "sensor/walabot.h"

#define UNUSED(var)

class Server_receiver : public ipme::sensor::Receiver {
  public:
    explicit Server_receiver(double& depth);
    virtual ~Server_receiver();

    virtual void process(const ipme::sensor::TargetId sensor_id,
                         const ipme::sensor::Reading& reading) override;

    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) override;

    void start_video_capture();

  private:
    double compute_factor();

    double& depth_;
    std::thread camera_thread_;
    std::mutex mutex_;

    const double MaxAllowedDepth = 500.0;
};

int main(int argc, char* argv[])
{
    if(argc && argv) {
    }

    using Settings = ipme::sensor::Walabot::Settings;
    Settings::Radial radial{20, 600, 1};
    Settings::Theta theta{-15, 15, 1};
    Settings::Phi phi{-25, 25, 3};
    Settings settings{radial, theta, phi};

    double depth{100.0};
    auto receiver = std::make_shared<Server_receiver>(depth);

    ipme::sensor::Walabot walabot{receiver, settings, true};
    walabot.record_targets(1000);
}

Server_receiver::Server_receiver(double& depth)
    : depth_{depth}, camera_thread_{[&] { start_video_capture(); }}
{
}

Server_receiver::~Server_receiver()
{
    camera_thread_.join();
}

void Server_receiver::process(const ipme::sensor::TargetId sensor_id,
                              const ipme::sensor::Reading& reading)
{
    if(sensor_id == 0) {
        std::lock_guard<std::mutex> lock{mutex_};
        depth_ = reading.z;
    }
}

void Server_receiver::process_image_slice(int* UNUSED(raster_image),
                                          int UNUSED(size_x),
                                          int UNUSED(size_y),
                                          double UNUSED(depth),
                                          double UNUSED(power))
{
    //
}

void Server_receiver::start_video_capture()
{
    cv::namedWindow("Video Capture", cv::WINDOW_AUTOSIZE);
    cv::VideoCapture capture;
    capture.open(0);

    if(!capture.isOpened()) {
        std::cerr << "Couldn't open" << std::endl;
        exit(EXIT_FAILURE);
    }

    cv::Mat frame;
    cv::Size size{0, 0};
    for(;;) {
        capture >> frame;
        if(frame.empty()) {
            break;
        }

        const auto orig_size = frame.size();
        if(size.width == 0 || size.height == 0) {
            size.width = orig_size.width;
            size.height = orig_size.height;
        } else {
            const auto factor = compute_factor();
            const auto width =
                static_cast<int>(static_cast<double>(orig_size.width) * factor);
            const auto height = static_cast<int>(
                static_cast<double>(orig_size.height) * factor);

            const auto ratio = static_cast<double>(height * width) /
                               static_cast<double>(size.width * size.height);
            if(ratio > 0.01 && ratio < 2.0) {
                size.width = width;
                size.height = height;
            }
            std::cout << "width: " << width << ", height: " << height
                      << ", size.width: " << size.width
                      << ", size.height: " << size.height
                      << ", ratio: " << ratio << std::endl;
        }

        cv::Mat transformed_frame;
        cv::resize(frame, transformed_frame, size);
        cv::imshow("Video Capture", transformed_frame);
        cv::waitKey(100);
    }
}

double Server_receiver::compute_factor()
{
    std::lock_guard<std::mutex> lock{mutex_};
    double scale = std::min(std::max(0.0, depth_), MaxAllowedDepth);
    return (scale * 1.5 / MaxAllowedDepth) + 0.5;
}
