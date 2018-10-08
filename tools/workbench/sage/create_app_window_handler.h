#ifndef IPME_WB_SAGE_CREATE_APP_WINDOW_HANDLER_H
#define IPME_WB_SAGE_CREATE_APP_WINDOW_HANDLER_H

#include <string_view>

#include "default_sage_message_handler.h"

namespace ipme::wb::sage {
class Create_app_window_handler : public Default_sage_message_handler {
public:
    inline Create_app_window_handler(const std::string& alias)
        : Default_sage_message_handler{"createAppWindow", alias}
    {
    }

    void dispatch(const utils::Json& message) override;
};
} // namespace ipme::wb::sage
#endif // IPME_WB_SAGE_CREATE_APP_WINDOW_HANDLER_H
