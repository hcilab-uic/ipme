#ifndef IPME_WB_SAGE_MESSAGE_HANDLER_H
#define IPME_WB_SAGE_MESSAGE_HANDLER_H

#include <istream>
#include <memory>
#include <string_view>

#include "sage/sage_element_container.h"
#include "utils/json.h"

namespace ipme {
namespace wb {
class Sage_message_handler {
public:
    virtual std::string_view name() const = 0;
    virtual std::string_view alias() const = 0;
    virtual void handle_message(std::istream& in) = 0;
    virtual std::string generate_registration_message() = 0;
    virtual void dispatch(const utils::Json& message) = 0;
    virtual void set_element_container(
        std::shared_ptr<ipme::wb::sage::Sage_element_container> container) = 0;
};
} // namespace wb
} // namespace ipme

#endif // IPME_WB_SAGE_MESSAGE_HANDLER_H
