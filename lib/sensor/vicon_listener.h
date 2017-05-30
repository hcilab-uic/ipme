#ifndef IPME_SENSOR_VICONLISTENER_H
#define IPME_SENSOR_VICONLISTENER_H

#include <memory>

#include <connector/omicronConnectorClient.h>

#include "vicon_datahandler.h"

namespace ipme {
namespace sensor {
namespace oc = omicronConnector;

class Vicon_listener : public oc::IOmicronConnectorClientListener {
  public:
    /**
     * Constructor
     */
    Vicon_listener(const Vicon_datahandler& handler);

    /*! @copydoc oc::IOmicronConnectorClientListener::onEvent
     */
    virtual void onEvent(const oc::EventData& event) override;

  private:
    const Vicon_datahandler& handler_;
};

} // namespace sensor
} // namespace ipme
#endif // IPME_SENSOR_VICONLISTENER_H
