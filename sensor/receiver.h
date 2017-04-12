#ifndef CDI_SENSOR_RECEIVER_H
#define CDI_SENSOR_RECEIVER_H

#include "sensor.h"

namespace cdi {
namespace sensor {
class Receiver {
  public:
    virtual ~Receiver() = default;
    virtual void process(const TargetId sensor_id, const Reading& reading) = 0;
    virtual void process_image_slice(int* raster_image, int size_x, int size_y,
                                     double depth, double power) = 0;
};
}
} // namespace cdi
#endif // CDI_SENSOR_RECEIVER_H
