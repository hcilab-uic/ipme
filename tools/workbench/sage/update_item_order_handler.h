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

#ifndef IPME_WB_SAGE_UPDATE_ITEM_ORDER_HANDLER_H
#define IPME_WB_SAGE_UPDATE_ITEM_ORDER_HANDLER_H

#include "default_sage_message_handler.h"

namespace ipme::wb::sage {
class Update_item_order_handler : public Default_sage_message_handler {
public:
    inline Update_item_order_handler(const std::string& alias)
        : Default_sage_message_handler{"updateItemOrder", alias}
    {
    }

    void dispatch(const std::string& display_id,
                  const utils::Json& message) override;
};
} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_UPDATE_ITEM_ORDER_HANDLER_H
