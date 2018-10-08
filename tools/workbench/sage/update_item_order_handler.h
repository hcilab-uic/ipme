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

    void dispatch(const utils::Json& message) override;
};
} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_UPDATE_ITEM_ORDER_HANDLER_H
