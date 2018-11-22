#include "update_item_order_handler.h"

#include "utils/logger.h"

namespace ipme::wb::sage {
void Update_item_order_handler::dispatch(const std::string& display_id,
                                         const utils::Json& message)
{
    INFO() << "update item order message from " << display_id << "\n"
           << message.get("d");
}

} // namespace ipme::wb::sage
