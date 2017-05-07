#include <cmath>

#include <WalabotAPI.h>

#include "utils/logger.h"

namespace cdi {
namespace sensor {

constexpr double PI = 3.1415926535897;

template <typename Point_t>
const boost::filesystem::path Walabot3d<Point_t>::settings_folder{
    "/var/lib/walabot"};

template <typename Point_t>
Walabot3d<Point_t>::Walabot3d(
    const std::shared_ptr<Receiver3d<Point_t>>& receiver,
    const Settings& settings, bool moving_target)
    : receiver_{receiver}, settings_{settings},
      theta_span_{settings.theta_degrees.max - settings.theta_degrees.min},
      phi_span_{settings.phi_degrees.max - settings.phi_degrees.min},
      r_span_{settings.radial_cm.max - settings.radial_cm.min}
{
    Walabot_SetSettingsFolder(
        const_cast<char*>(settings_folder.string().c_str()));
    Walabot_ConnectAny();
    Walabot_SetProfile(PROF_SENSOR);

    Walabot_SetArenaR(settings_.radial_cm.min, settings_.radial_cm.max,
                      settings_.radial_cm.resolution);
    Walabot_SetArenaTheta(settings_.theta_degrees.min,
                          settings_.theta_degrees.max,
                          settings_.theta_degrees.resolution);
    Walabot_SetArenaPhi(settings_.phi_degrees.min, settings_.phi_degrees.max,
                        settings_.phi_degrees.resolution);

    Walabot_SetDynamicImageFilter(moving_target ? FILTER_TYPE_MTI
                                                : FILTER_TYPE_NONE);
    double threshold{};
    Walabot_GetThreshold(&threshold);
    Walabot_SetThreshold(0);
    Walabot_Start();
    if(!moving_target) {
        Walabot_StartCalibration();
    }
}

template <typename Point_t>
Walabot3d<Point_t>::~Walabot3d()
{
    Walabot_Stop();
    Walabot_Disconnect();
}

template <typename Point_t>
void Walabot3d<Point_t>::record(const int iterations)
{
    APP_STATUS app_status;
    double calibration_process{0.0};
    int* raster_image = nullptr;
    int size_x{0};
    int size_y{0};
    int size_z{0};
    double power{0};

    for(int i = 0;; ++i) {
        Walabot_GetStatus(&app_status, &calibration_process);
        Walabot_Trigger();
        Walabot_GetRawImage(&raster_image, &size_x, &size_y, &size_z, &power);

        process_snapshot(raster_image, size_x, size_y, size_z, power);

        if(iterations > 0 && i > iterations) {
            break;
        }
    }
}

static double degrees_to_radians(double degrees)
{
    return (degrees * PI) / 180.0;
}

template <typename Point_t>
void cdi::sensor::Walabot3d<Point_t>::process_snapshot(int* raster_image,
                                                       int size_x, int size_y,
                                                       int size_z, double power)
{
    for(int i = 0; i < size_x; ++i) {
        for(int j = 0; j < size_y; ++j) {
            for(int k = 0; k < size_z; ++k) {
                const double theta = settings_.theta_degrees.min +
                                     i * theta_span_ / (size_x - 1);
                const double phi =
                    settings_.phi_degrees.min + j * phi_span_ / (size_y - 1);
                const double r =
                    settings_.radial_cm.min + k * r_span_ / (size_z - 1);

                const double sin_theta = sin(degrees_to_radians(theta));
                const double cos_theta = cos(degrees_to_radians(theta));

                const double sin_phi = sin(degrees_to_radians(phi));
                const double cos_phi = cos(degrees_to_radians(phi));

                const int index = (size_y * size_z * i) + (size_z * j) * k;
                int raster = raster_image[index];
                //                if(raster < 0 || raster > 255) {
                //                    continue;
                //                }

                Point_t point{r * sin_theta * cos_phi, r * sin_theta * sin_phi,
                              r * cos_theta, raster, power};
                receiver_->process(point);
                DEBUG() << "x: " << point.x << ", y: " << point.y
                        << ", z: " << point.z << ", raster: " << point.raster
                        << ", power: " << point.raster;
            }
        }
    }
}

} // namespace sensor
} // namespace cdi
