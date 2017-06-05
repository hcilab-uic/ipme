#ifndef CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H
#define CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H

#include <boost/filesystem.hpp>

#include "receiver3d.h"
#include "visualization/polydata.h"

namespace ipme {
namespace sensor {
template <typename Point_t>
class Walabot_receiver_writer : public Receiver3d<Point_t> {
  public:
    using Data_t = visualization::Polydata<Point_t>;
    /**
     * @brief Walabot_receiver_writer constructor
     */
    Walabot_receiver_writer()
        : data_{std::make_unique<Data_t>()}
    {
    }

    /**
     * virtual destructor
     */
    virtual ~Walabot_receiver_writer() = default;

    /*! @copydoc Receiver3d::process(const Point_t&)
     */
    virtual void process(const Point_t& point);

    /**
     * @brief write function to write data to file
     * @param output_filename name of the file to write to
     */
    void write(const boost::filesystem::path& output_filename);

  private:
    std::unique_ptr<Data_t> data_;
};

} // namespace sensor
} // namespace ipme

#include "walabot_receiver_writer.ipp"

#endif // CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H