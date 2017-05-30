#ifndef IPME_SENSOR_VICONDATAHANDLER_H
#define IPME_SENSOR_VICONDATAHANDLER_H

#include "connector/omicronConnectorClient.h"

namespace ipme {
namespace sensor {
namespace oc = omicronConnector;

/**
 * @brief The Vicon_datahandler interface
 */
class Vicon_datahandler {
  public:
    /**
     * Virtual destructor
     */
    virtual ~Vicon_datahandler() = default;

    /**
     * Handle trace event
     * @param[in] event data
     */
    virtual void handle_trace(const oc::EventData& event) = 0;

    /**
     * Handle untrace event
     * @param[in] event data
     */
    virtual void handle_untrace(const oc::EventData& event) = 0;

    /**
     * Handle update event
     * @param[in] event data
     */
    virtual void handle_update(const oc::EventData& event) = 0;

    /**
     * Handle down event
     * @param[in] event data
     */
    virtual void handle_up(const oc::EventData& event) = 0;

    /**
     * Handle u event
     * @param[in] event data
     */
    virtual void handle_down(const oc::EventData& event) = 0;
};
} // namespace sensor
} // namespace ipme

#endif // IPME_SENSOR_VICONDATAHANDLER_H
