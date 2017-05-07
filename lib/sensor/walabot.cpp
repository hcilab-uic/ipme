#include "walabot.h"

#include <iostream>

#include <WalabotAPI.h>

namespace cdi {
namespace sensor {
const std::string Walabot::settings_folder{"/var/lib/walabot"};

Walabot::Walabot(const std::shared_ptr<Receiver> receiver,
                 const Settings& settings, bool moving_target)
    : receiver_{receiver}, settings_{settings}, moving_target_{moving_target}
{
    Walabot_SetSettingsFolder(const_cast<char*>(settings_folder.c_str()));
    Walabot_ConnectAny();
    Walabot_SetProfile(PROF_SENSOR);

    Walabot_SetArenaR(settings_.cm.min, settings_.cm.max, settings_.cm.res);
    Walabot_SetArenaTheta(settings_.degrees.min, settings_.degrees.max,
                          settings_.degrees.res);
    Walabot_SetArenaPhi(settings_.phi.min, settings_.phi.max,
                        settings_.phi.res);

    Walabot_SetDynamicImageFilter(moving_target_ ? FILTER_TYPE_MTI
                                                 : FILTER_TYPE_NONE);
    double threshold{};
    Walabot_GetThreshold(&threshold);
    std::cout << threshold << std::endl;
    Walabot_SetThreshold(0);
    Walabot_Start();
    if(!moving_target_) {
        Walabot_StartCalibration();
    }
}

Walabot::~Walabot()
{
    Walabot_Stop();
    Walabot_Disconnect();
}

void Walabot::record_image_slice(const int iterations) const
{
    APP_STATUS app_status;
    double calibration_process{};
    int* raster_image = nullptr;
    int size_x;
    int size_y;
    double slice_depth;
    double power;

    for(int i = 0;; ++i) {
        Walabot_GetStatus(&app_status, &calibration_process);
        Walabot_Trigger();
        Walabot_GetRawImageSlice(&raster_image, &size_x, &size_y, &slice_depth,
                                 &power);
        receiver_->process_image_slice(raster_image, size_x, size_y,
                                       slice_depth, power);

        if(iterations > 0 && i > iterations) {
            break;
        }
    }
}

void Walabot::record_targets(const int iterations) const
{
    APP_STATUS app_status{STATUS_DISCONNECTED};
    double calibration_process{};
    SensorTarget* targets = nullptr;
    int num_targets{};

    while(app_status == STATUS_DISCONNECTED) {
        Walabot_GetStatus(&app_status, &calibration_process);
        std::cout << "Calibration complete: " << calibration_process
                  << std::endl;
    }

    for(int i = 0; i == 0 || i < iterations; ++i) {
        Walabot_Trigger();
        Walabot_GetSensorTargets(&targets, &num_targets);
        for(int index = 0; index < num_targets; ++index) {
            receiver_->process(
                static_cast<TargetId>(index),
                Reading{targets[index].xPosCm, targets[index].yPosCm,
                        targets[index].zPosCm, targets[index].amplitude});
        }
    }
}

} // namespace sensor
} // namespace cdi
