#ifndef CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H
#define CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H

#include "receiver3d.h"

namespace cdi {
namespace sensor {
template<typename Point_t>
class Walabot_receiver_writer: public Receiver3d<Point_t> {
public:
    virtual ~Walabot_receiver_writer() = default;

    virtual void process(const Point_t& point);
};

} // namespace sensor
} // namespace cdi

#include "receiver3d.ipp"

#endif // CDI_LIB_SENSOR_WALABOTRECEIVERWRITER_H
