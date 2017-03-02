#ifndef CDI_SENSOR_RECEIVER_H
#define CDI_SENSOR_RECEIVER_H

#include "sensor.h"

namespace cdi {
namespace sensor {
class Receiver {
  public:
    virtual void process(const TSensorId sensor_id, const Reading& reading) = 0;
};
}
} // namespace cdi
#endif // CDI_SENSOR_RECEIVER_H
