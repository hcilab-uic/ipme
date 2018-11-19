#include "update_item_order_handler.h"

#include "utils/logger.h"

namespace ipme::wb::sage {
void Update_item_order_handler::dispatch(const utils::Json& message)
{
    INFO() << message.get("d");
}

} // namespace ipme::wb::sage
