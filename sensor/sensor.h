#ifndef CDI_SENSOR_SENSOR_H
#define CDI_SENSOR_SENSOR_H

namespace cdi {
namespace sensor {

using TargetId = int;

struct Reading {
    Reading(const double x_, const double y_, const double z_,
            const double amplitude_)
        : x{x_}, y{y_}, z{z_}, amplitude{amplitude_}
    {
    }

    double x;
    double y;
    double z;
    double amplitude;
};
}
} // namespace cdi
#endif // CDI_SENSOR_SENSOR_H
