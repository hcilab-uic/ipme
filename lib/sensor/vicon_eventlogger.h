#ifndef IPME_SENSOR_VICON_EVENTLOGGER_H
#define IPME_SENSOR_VICON_EVENTLOGGER_H

#include "vicon_datahandler.h"

namespace ipme {
namespace sensor {
class Vicon_eventlogger : public Vicon_datahandler {
  public:
    /*! @copydoc Vicon_datahandler::handle_trace(const oc::EventData&)
     */
    virtual void handle_trace(const oc::EventData& event) const override;

    /*! @copydoc Vicon_datahandler::handle_untrace(const oc::EventData&)
     */
    virtual void handle_untrace(const oc::EventData& event) const override;

    /*! @copydoc Vicon_datahandler::handle_update(const oc::EventData&)
     */
    virtual void handle_update(const oc::EventData& event) const override;

    /*! @copydoc Vicon_datahandler::handle_up(const oc::EventData&)
     */
    virtual void handle_up(const oc::EventData& event) const override;

    /*! @copydoc Vicon_datahandler::handle_down(const oc::EventData&)
     */
    virtual void handle_down(const oc::EventData& event) const override;
};
} // namespace sensor
} // namespace ipme

#endif // IPME_SENSOR_VICON_EVENTLOGGER_H
