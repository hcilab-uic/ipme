#include <iostream>
#include <memory>
#include <sstream>

#include "sensor.h"
#include "walabot.h"

class MyReceiver : public cdi::sensor::Receiver {
  public:
    virtual void process(const cdi::sensor::TSensorId sensor_id,
                         const cdi::sensor::Reading& reading) override;
};

void MyReceiver::process(const cdi::sensor::TSensorId sensor_id,
                         const cdi::sensor::Reading& reading)
{
    std::cout << "target: " << sensor_id << ", [X: " << reading.x << "], [Y: "
	      << reading.y << "], [Z: " << reading.z << "], [Amplitude: "
	      << reading.amplitude << "]" << std::endl;
}

int main()
{
    using Settings = cdi::sensor::Walabot::Settings;
    Settings settings{Settings::Inner{30, 200, 3}, Settings::Inner{-15, 15, 5},
                      Settings::Inner{-60, 60, 5}};

    auto receiver = std::make_shared<MyReceiver>();

    cdi::sensor::Walabot walabot{receiver, settings, true};
    walabot.record(100);
}
