#include <boost/asio.hpp>
#include <cstring>
#include <mutex>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>

struct Reading_packet {
    // Trying to build this independent of sensor since we might have to
    // build this on Mac and Mac has no sensor SDK
    Reading_packet(double x_, double y_, double z_, double amplitude_)
        : x{x_}, y{y_}, z{z_}, amplitude{amplitude_}
    {
    }

    double x = 0;
    double y = 0;
    double z = 0;
    double amplitude = 0;
};

class Signal_receiver {
  public:
    Signal_receiver(const std::string& host, short port);

    void start_receiving_packets();

  private:
    void start_capture();
    double compute_factor();

    const std::string host_;
    const short port_;
    std::thread camera_thread_;
    std::mutex mutex_;
    Reading_packet reading_;

    const double MaxAllowedDepth = 500.0;
};

int main(int argc, char* argv[])
{
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    Signal_receiver receiver{argv[1], static_cast<short>(atoi(argv[2]))};
    receiver.start_receiving_packets();
}

Signal_receiver::Signal_receiver(const std::string& host, short port)
    : host_{host}, port_{port}, camera_thread_{[&] { start_capture(); }},
      reading_{0., 0., 0., 0.}
{
}

void Signal_receiver::start_receiving_packets()
{
    try {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver{io_service};
        boost::asio::ip::tcp::resolver::query query{
            boost::asio::ip::tcp::v4(), host_, std::to_string(port_)};
        boost::asio::ip::tcp::resolver::iterator iterator =
            resolver.resolve(query);
        boost::asio::ip::tcp::socket socket{io_service};
        socket.connect(*iterator);

        for(;;) {
            char receive_buffer[sizeof(Reading_packet)];
            memset(&receive_buffer, 0, sizeof(receive_buffer));
            size_t receive_length = boost::asio::read(
                socket,
                boost::asio::buffer(receive_buffer, sizeof(Reading_packet)));

            if(receive_length == 0) {

                std::cerr << "Received " << receive_length << " length packet "
                          << std::endl;
            }

            std::lock_guard<std::mutex> lock{mutex_};
            memcpy(reinterpret_cast<char*>(&reading_), receive_buffer,
                   sizeof(reading_));
        }
    } catch(const std::runtime_error& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }
}

void Signal_receiver::start_capture()
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
            const auto factor = (0.5 + compute_factor());
            const auto width =
                static_cast<int>(static_cast<double>(orig_size.width) * factor);
            const auto height = static_cast<int>(
                static_cast<double>(orig_size.height) * factor);

            const auto ratio = static_cast<double>(height * width) /
                               static_cast<double>(size.width * size.height);
            if(ratio > 0.01 && ratio < 20.0) {
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

double Signal_receiver::compute_factor()
{
    std::lock_guard<std::mutex> lock{mutex_};
    double scale = std::min(std::max(0.0, reading_.z), MaxAllowedDepth);
    return scale / MaxAllowedDepth;
}
