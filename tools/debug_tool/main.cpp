#include <iostream>
#include <memory>
#include <sstream>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "sensor.h"
#include "walabot.h"

// class MyReceiver : public cdi::sensor::Receiver {
//  public:
//    virtual void process(const cdi::sensor::TSensorId sensor_id,
//                         const cdi::sensor::Reading& reading) override;
//};

// void MyReceiver::process(const cdi::sensor::TSensorId sensor_id,
//                         const cdi::sensor::Reading& reading)
//{
//    std::cout << "target: " << sensor_id << ", [X: " << reading.x
//              << "], [Y: " << reading.y << "], [Z: " << reading.z
//              << "], [Amplitude: " << reading.amplitude << "]" << std::endl;
//}

class SignalDisplayer : public cdi::sensor::Receiver {
  public:
    SignalDisplayer(int width, int height);

    virtual void process(const cdi::sensor::TSensorId sensor_id,
                         const cdi::sensor::Reading& reading) override;

    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) override;

  private:
    const int width_;
    const int height_;
    cv::Mat image_;
};

int main()
{
    using Settings = cdi::sensor::Walabot::Settings;
    Settings settings{Settings::Inner{30, 200, 3}, Settings::Inner{-15, 15, 5},
                      Settings::Inner{-60, 60, 5}};

    auto receiver = std::make_shared<SignalDisplayer>(570, 250);

    cdi::sensor::Walabot walabot{receiver, settings, true};
    walabot.record(0);
    std::cout << "sdasdf" << std::endl;
}

SignalDisplayer::SignalDisplayer(int width, int height)
    : width_{width}, height_{height}, image_{height, width, CV_8UC3}
{
}

void SignalDisplayer::process(const cdi::sensor::TSensorId /* sensor_id */,
                              const cdi::sensor::Reading& /* reading */)
{
    //    if(sensor_id) {
    //    }
    //    int x = (reading.x + 90) / 180 * 600;
    //    int y = (reading.y + 90) / 180 * 800;

    //    //    unsigned char red = 255 - reading.z;
    //    unsigned char blue = reading.z;
    //    image_.at<cv::Vec3b>(x, y) = cv::Vec3b{0, 0, blue};

    //    cv::Mat cm_img;
    //    cv::applyColorMap(image_, cm_img, cv::COLORMAP_JET);
    //    cv::imshow("", cm_img);
    //    cv::waitKey(1);
}

void SignalDisplayer::process_image_slice(int* raster_image, int size_x,
                                          int size_y, double depth,
                                          double power)
{
    const int pixel_width = width_ / size_x;
    const int pixel_height = height_ / size_y;
    std::cout << "Depth: " << depth << ", power: " << power << ", X: " << size_x
              << ", Y: " << size_y << ", pixel width: " << pixel_width
              << ", pixel_height: " << pixel_height << std::endl;
    for(int i = 0; i < size_x; ++i) {
        for(int j = 0; j < size_y; ++j) {
            int index = (size_x * j) + i;
            int dot = raster_image[index];
            //            unsigned char red = static_cast<unsigned char>(255 -
            //            dot);
            //            unsigned char green = 0;
            unsigned char blue = static_cast<unsigned char>(dot);

            for(int m = 0; m < pixel_height; ++m) {
                for(int n = 0; n < pixel_width; ++n) {
                    int y = (i * pixel_height) + m;
                    int x = (j * pixel_width) + n;
                    image_.at<cv::Vec3b>(x, y) = cv::Vec3b{blue, 0, 0};
                }
            }
        }
    }
    cv::Mat cm_img;
    cv::applyColorMap(image_, cm_img, cv::COLORMAP_JET);
    cv::imshow("", cm_img);
    //    cv::imshow("", image_);
    cv::waitKey(1);
}
