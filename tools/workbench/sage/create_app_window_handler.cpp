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
