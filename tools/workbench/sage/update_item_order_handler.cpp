#include "update_item_order_handler.h"

namespace ipme::wb::sage {
void Update_item_order_handler::dispatch(const utils::Json& message)
{
    std::cout << message.get("d") << "\n";
}

} // namespace ipme::wb::sage
