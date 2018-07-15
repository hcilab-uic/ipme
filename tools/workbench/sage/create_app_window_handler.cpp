#include "create_app_window_handler.h"

#include <iostream>

#include "utils/json.h"
#include "utils/logger.h"

namespace ipme::wb::sage {
void Create_app_window_handler::dispatch(const utils::Json& message)
{
    Default_sage_message_handler::container_->add_element(message);
    INFO() << "Created app window ";
    //    INFO() << message.to_string();
}

} // namespace ipme::wb::sage
