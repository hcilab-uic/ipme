#include <iostream>
#include <memory>
#include <sstream>

#include "sensor/sensor.h"
#include "sensor/walabot.h"

class MyReceiver : public cdi::sensor::Receiver {
  public:
    virtual void process(const cdi::sensor::TargetId sensor_id,
                         const cdi::sensor::Reading& reading) override;
    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) override;
};

void MyReceiver::process(const cdi::sensor::TargetId sensor_id,
                         const cdi::sensor::Reading& reading)
{
    std::cout << "target: " << sensor_id << ", [X: " << reading.x
              << "], [Y: " << reading.y << "], [Z: " << reading.z
              << "], [Amplitude: " << reading.amplitude << "]" << std::endl;
}

void MyReceiver::process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power)
{
    std::cout << "Depth: " << depth << ", power" << power << std::endl;
    for(int i = 0; i < size_x; ++i) {
        for(int j = 0; j < size_y; ++j) {
            std::cout << raster_image[i] << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

int main()
{
    using Settings = cdi::sensor::Walabot::Settings;
    Settings settings{Settings::Radial{30, 200, 3},
                      Settings::Radial{-15, 15, 5},
                      Settings::Radial{-60, 60, 5}};

    auto receiver = std::make_shared<MyReceiver>();

    cdi::sensor::Walabot walabot{receiver, settings, true};
    walabot.record_image(100);
}
