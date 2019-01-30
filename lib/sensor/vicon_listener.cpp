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

#include "vicon_listener.h"

#include <sstream>

#include "utils/logger.h"

namespace ipme {
namespace sensor {
Vicon_listener::Vicon_listener(std::shared_ptr<Vicon_datahandler> handler)
    : handler_{std::move(handler)}
{
}

void Vicon_listener::onEvent(const omicronConnector::EventData& event)
{
    switch(event.type) {
    case oc::EventData::Trace:
        handler_->handle_trace(event);
        break;

    case oc::EventData::Untrace:
        handler_->handle_untrace(event);
        break;

    case oc::EventData::Update:
        handler_->handle_update(event);
        break;

    case oc::EventData::Down:
        handler_->handle_down(event);
        break;

    case oc::EventData::Up:
        handler_->handle_up(event);
        break;

    default:
        std::stringstream ss;
        ss << "Unknown event of type: " << event.type << " received";
        ERROR() << ss.str();
    }

    ++event_count_;
}

} // namespace sensor
} // namespace ipme
