#ifndef CDI_LIB_SENSOR_WALABOT3D_H
#define CDI_LIB_SENSOR_WALABOT3D_H

#include <boost/filesystem.hpp>
#include <memory>

#include "receiver3d.h"
#include "sensor.h"

namespace cdi {
namespace sensor {
template <typename Point_t>
class Walabot3d : public Sensor {
  public:
    /**
     * @brief Walabot3d Constructor
     * @param receiver polymorphic handle
     * @param settings sensor settings
     */
    Walabot3d(const std::shared_ptr<Receiver3d<Point_t>>& receiver,
              const Settings& settings, bool moving_target);

    /*! @copydoc Sensor::~Sensor()
     */
    virtual ~Walabot3d();

    /*! @copydoc Sensor::record(const int)
     */
    virtual void record(const int iterations) override;

  private:
    void process_snapshot(int* raster_image, int size_x, int size_y, int size_z,
                          double power);

    std::unique_ptr<Receiver3d<Point_t>> receiver_;
    const Settings settings_;
    const Spec_range::Min theta_span_;
    const Spec_range::Min phi_span_;
    const Spec_range::Min r_span_;

    static const boost::filesystem::path settings_folder;
};
} // namespace sensor
} // namespace cdi

#include "walabot3d.ipp"
#endif // CDI_LIB_SENSOR_WALABOT3D_H
