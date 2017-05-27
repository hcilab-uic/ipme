#include <memory>

namespace ipme {
namespace sensor {
//template <typename Point_t>
//Walabot_receiver_writer<Point_t>::Walabot_receiver_writer()
//    : data_{std::make_unique<visualization::Polydata<Point_t>>()}
//{
//}

template <typename Point_t>
void Walabot_receiver_writer<Point_t>::process(const Point_t& point)
{
    data_->add_point(point);
}

template <typename Point_t>
void Walabot_receiver_writer<Point_t>::write(
    const boost::filesystem::path& output_filename)
{
    data_->write_file(output_filename);
}

} // namespace sensor
} // namespace ipme
