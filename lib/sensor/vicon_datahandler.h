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
