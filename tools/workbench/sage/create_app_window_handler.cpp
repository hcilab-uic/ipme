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

#include "create_app_window_handler.h"

#include <iostream>

#include "utils/json.h"
#include "utils/logger.h"

namespace ipme::wb::sage {
void Create_app_window_handler::dispatch(const std::string& display_id,
                                         const utils::Json& message)
{
    Default_sage_message_handler::container_->add_element(display_id, message);
    INFO() << "Created app window on display " << display_id;
}

} // namespace ipme::wb::sage
