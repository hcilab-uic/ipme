#ifndef CDI_LIB_SENSOR_SENSOR_H
#define CDI_LIB_SENSOR_SENSOR_H

#include <string>
#include <vector>

namespace cdi {
namespace sensor {

using TargetId = int;

/**
 * @brief Structure representing a single reading of a sensor
 */
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

/**
 * @brief The Spec_range struct to store settings ranges and resolution
 */
struct Spec_range {
    using Min = double;
    using Max = double;
    using Resolution = double;

    Spec_range(const Min min_, const Max max_, const Resolution resolution_);
    Spec_range(const std::vector<std::string>& range_vector);
    Spec_range(const std::string& range, const std::string& delim = ":");

    Min min;
    Max max;
    Resolution resolution;
};

/**
 * @brief The Settings struct to store sensor setting
 */
struct Settings {
    using Radial = Spec_range;
    using Theta = Spec_range;
    using Phi = Spec_range;

    Settings(const Radial& radial_cm_, const Theta& theta_degrees_,
             const Phi& phi_degrees_);

    Radial radial_cm;
    Theta theta_degrees;
    Phi phi_degrees;
};

/**
 * @brief The Sensor interface
 */
class Sensor {
  public:
    /**
     * virtual destructor
     */
    virtual ~Sensor() = default;

    /**
     * @brief record a function that does that takes actual readings from the
     * sensor
     * @param iterations number of iterations to take. 0 means no limit.
     */
    virtual void record(const int iterations) = 0;
};

} // namespace sensor
} // namespace cdi
#endif // CDI_LIB_SENSOR_SENSOR_H
