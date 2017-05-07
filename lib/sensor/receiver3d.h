#ifndef CDI_LIB_SENSOR_RECEIVER3D_H
#define CDI_LIB_SENSOR_RECEIVER3D_H

namespace cdi {
namespace sensor {
template <typename TPoint>
class Receiver3d {
  public:
    virtual ~Receiver3d() = default;

    /**
     * @brief process the point information that is passed by the sensor
     * @param point 3d point with or without other attributes
     */
    virtual void process(const TPoint& point) = 0;
};
} // namespace sensor
} // namespace cdi
#endif // CDI_LIB_SENSOR_RECEIVER3D_H
