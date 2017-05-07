#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "sensor/sensor.h"
#include "sensor/walabot.h"

class SignalDisplayer : public cdi::sensor::Receiver {
  public:
    SignalDisplayer(int width, int height);

    virtual void process(const cdi::sensor::TargetId sensor_id,
                         const cdi::sensor::Reading& reading) override;

    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) override;

  private:
    const int width_;
    const int height_;
    cv::Mat image_;
};

SignalDisplayer::SignalDisplayer(int width, int height)
    : width_{width}, height_{height}, image_{height, width, CV_8UC3}
{
}

void SignalDisplayer::process(const cdi::sensor::TargetId /* sensor_id */,
                              const cdi::sensor::Reading& /* reading */)
{
}

void SignalDisplayer::process_image_slice(int* raster_image, int size_x,
                                          int size_y, double depth,
                                          double power)
{
    const int pixel_width = width_ / size_x;
    const int pixel_height = height_ / size_y;
    if(depth && power) {
    }
    //    std::cout << "Depth: " << depth << ", power: " << power << ", X: " <<
    //    size_x
    //              << ", Y: " << size_y << ", p w: " << pixel_width
    //              << ", p h: " << pixel_height << "\n";
    for(int i = 0; i < size_x; ++i) {
        for(int j = 0; j < size_y; ++j) {
            int index = (size_x * j) + i;
            int dot = raster_image[index];
            unsigned char red = static_cast<unsigned char>(255 - dot);
            unsigned char green = static_cast<unsigned char>(dot);
            unsigned char blue = static_cast<unsigned char>(dot);
            const auto color_dot = cv::Vec3b{red, green, blue};

            //            double phi = -90.0 + (i * (90 + 90)) / (size_x - 1);
            //            double R = 1 + j * 499.0 / (size_y - 1);
            //            int y = R * std::sin(phi);
            //            int x = R * std::cos(phi);
            //            image_.at<cv::Vec3b>(y, x) = color_dot;
            for(int m = 0; m < pixel_width; ++m) {
                for(int n = 0; n < pixel_height; ++n) {
                    int y = (j * pixel_height) + n;
                    int x = (i * pixel_width) + m;
                    image_.at<cv::Vec3b>(y, x) = color_dot;
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

int main()
{
    using Settings = cdi::sensor::Walabot::Settings;
    Settings::Radial radial{1, 500, 1};
    Settings::Theta theta{0, 180, 1};
    Settings::Phi phi{-90, 90, 3};
    Settings settings{radial, theta, phi};

    auto receiver = std::make_shared<SignalDisplayer>(500, 61);

    cdi::sensor::Walabot walabot{receiver, settings, true};
    walabot.record_image(0);
    std::cout << "sdasdf" << std::endl;
}
