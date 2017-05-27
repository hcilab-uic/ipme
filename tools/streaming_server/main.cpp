#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iostream>
#include <mutex>
#include <thread>

#include "sensor/sensor.h"
#include "sensor/walabot.h"

#define UNUSED(variable)

class Server_receiver : public ipme::sensor::Receiver {
  public:
    Server_receiver(short port);
    virtual ~Server_receiver() = default;
    virtual void process(const ipme::sensor::TargetId target_id,
                         const ipme::sensor::Reading& reading) override;

    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) override;

  private:
    void start_server();
    void session(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr);

    boost::asio::io_service io_service_;
    std::mutex mutex_;
    const short port_;
    ipme::sensor::Reading reading_;
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

    auto receiver = std::make_shared<Server_receiver>(5123);

    ipme::sensor::Walabot walabot{receiver, settings, true};
    walabot.record_targets(1000);
}

Server_receiver::Server_receiver(short port)
    : port_{port}, reading_{0., 0., 0., 0.}
{
}

void Server_receiver::process(const ipme::sensor::TargetId sensor_id,
                              const ipme::sensor::Reading& reading)
{
    if(sensor_id == 0) {
        std::cout << reading.x << ", " << reading.y << ", " << reading.z
                  << std::endl;
        std::lock_guard<std::mutex> lock{mutex_};
        reading_ = reading;
    }
}

void Server_receiver::process_image_slice(int* UNUSED(raster_image),
                                          int UNUSED(size_x),
                                          int UNUSED(size_y),
                                          double UNUSED(depth),
                                          double UNUSED(power))
{
    // FIXME
}

void Server_receiver::start_server()
{
    boost::asio::ip::tcp::acceptor acceptor{io_service_,
                                            boost::asio::ip::tcp::v4(), port_};
    for(;;) {
        auto socket(
            std::make_shared<boost::asio::ip::tcp::socket>(io_service_));
        std::thread t{[&] { session(socket); }};
    }
}

void Server_receiver::session(
    std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    try {
        for(;;) {
            char data[sizeof(ipme::sensor::Reading)];
            boost::system::error_code error;

            std::lock_guard<std::mutex> lock{mutex_};
            memcpy(data, reinterpret_cast<char*>(&reading_), sizeof(data));
            boost::asio::write(*socket,
                               boost::asio::buffer(data, sizeof(data)));
        }
    } catch(std::exception ex) {
        std::cerr << "Exception in thread: " << ex.what() << std::endl;
    }
}
