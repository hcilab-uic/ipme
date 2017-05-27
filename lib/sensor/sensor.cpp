#include "sensor.h"

#include "utils/string_utils.h"

namespace ipme {
namespace sensor {

Spec_range::Spec_range(const Spec_range::Min min_, const Spec_range::Max max_,
                       const Spec_range::Resolution resolution_)
    : min{min_}, max{max_}, resolution{resolution_}
{
}

Spec_range::Spec_range(const std::vector<std::string>& range_vector)
    : Spec_range(utils::to_double(range_vector[0]),
                 utils::to_double(range_vector[1]),
                 utils::to_double(range_vector[2]))
{
}

Spec_range::Spec_range(const std::string& range, const std::string& delim)
    : Spec_range(utils::split_string(range, delim))
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
} // namespace ipme
