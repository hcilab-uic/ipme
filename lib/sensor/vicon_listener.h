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
    Vicon_listener(std::shared_ptr<Vicon_datahandler> handler);

    /**
     * virtual destructor
     */
    virtual ~Vicon_listener() = default;

    /**
     * Accessor function for number of handled events
     * @return event count
     */
    inline unsigned int event_count() const
    {
        return event_count_;
    }

    /*! @copydoc oc::IOmicronConnectorClientListener::onEvent
     */
    virtual void onEvent(const oc::EventData& event) override;

private:
    unsigned int event_count_ = 0;
    std::shared_ptr<Vicon_datahandler> handler_;
};

} // namespace sensor
} // namespace ipme
#endif // IPME_SENSOR_VICONLISTENER_H
