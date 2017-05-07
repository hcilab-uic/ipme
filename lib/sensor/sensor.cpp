#include "sensor.h"

namespace cdi {
namespace sensor {

Spec_range::Spec_range(const Spec_range::Min min_, const Spec_range::Max max_,
                       const Spec_range::Resolution resolution_)
    : min{min_}, max{max_}, resolution{resolution_}
{
}

Settings::Settings(const Settings::Radial& radial_cm_,
                   const Settings::Theta& theta_degrees_,
                   const Settings::Phi& phi_degrees_)
    : radial_cm{radial_cm_}, theta_degrees{theta_degrees_},
      phi_degrees{phi_degrees_}
{
}

} // namespace sensor
} // namespace cdi
