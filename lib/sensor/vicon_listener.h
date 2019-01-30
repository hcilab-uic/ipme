/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
