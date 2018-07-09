#ifndef IPME_SENSOR_VRPN_HANDLER_H
#define IPME_SENSOR_VRPN_HANDLER_H

#include <string_view>

#include "data/scene.h"
#include "sensor/vicon_datahandler.h"

namespace ipme {
namespace sensor {
class Vrpn_handler : public Vicon_datahandler {
public:
    Vrpn_handler(std::shared_ptr<data::Scene> scene);

    virtual ~Vrpn_handler() = default;

    /*! @copydoc Vicon_datahandler::handle_trace(const oc::EventData&)
     */
    virtual void handle_trace(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_untrace(const oc::EventData&)
     */
    virtual void handle_untrace(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_update(const oc::EventData&)
     */
    virtual void handle_update(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_up(const oc::EventData&)
     */
    virtual void handle_up(const oc::EventData& event) override;

    /*! @copydoc Vicon_datahandler::handle_down(const oc::EventData&)
     */
    virtual void handle_down(const oc::EventData& event) override;

private:
    std::shared_ptr<data::Scene> scene_;
};

} // namespace sensor
} // namespace ipme

#endif // IPME_SENSOR_VRPN_HANDLER_H
