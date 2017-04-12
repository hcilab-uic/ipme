#ifndef CDI_SENSOR_WALABOT_H
#define CDI_SENSOR_WALABOT_H

#include <memory>
#include <string>

#include "receiver.h"

namespace cdi {
namespace sensor {
class Walabot {
  public:
    struct Settings {
        struct Inner {
            Inner(double min_, double max_, double res_)
                : min{min_}, max{max_}, res{res_}
            {
            }
            double min;
            double max;
            double res;
        };

        //        class Radial : public Inner {
        //        };
        using Radial = Inner;
        using Theta = Inner;
        using Phi = Inner;

        Settings(const Radial& cm_, const Theta& degrees_, const Phi& phi_)
            : cm{cm_}, degrees{degrees_}, phi{phi_}
        {
        }

        Radial cm;
        Theta degrees;
        Phi phi;
    }; // struct Settings

    Walabot(const std::shared_ptr<Receiver> receiver, const Settings& settings,
            bool moving_target);
    virtual ~Walabot();
    void record_image(const int iterations) const;
    void record_targets(const int iterations) const;

  private:
    std::shared_ptr<Receiver> receiver_;
    const Settings settings_;
    const bool moving_target_;

    static const std::string settings_folder;
}; // class Walabot

} // namespace sensor
} // namespace cdi

#endif // CDI_SENSOR_WALABOT_H
