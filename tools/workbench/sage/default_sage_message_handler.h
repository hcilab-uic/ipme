#ifndef DEFAULT_SAGE_MESSAGE_HANDLER_H
#define DEFAULT_SAGE_MESSAGE_HANDLER_H

#include <string_view>

#include "sage_message_handler.h"
#include "utils/json.h"

namespace ipme::wb::sage {
class Default_sage_message_handler : public ipme::wb::Sage_message_handler {
public:
    inline Default_sage_message_handler(const std::string& name,
                                        const std::string& alias)
        : name_{name}, alias_{alias}
    {
    }

    inline std::string_view name() const override
    {
        return name_;
    }

    std::string_view alias() const override
    {
        return alias_;
    }

    void handle_message(std::istream& in) override;

    std::string generate_registration_message() override;

    void dispatch(const utils::Json& message) override;

    void set_element_container(
        std::shared_ptr<ipme::wb::sage::Sage_element_container> container)
        override;

protected:
    std::shared_ptr<ipme::wb::sage::Sage_element_container> container_;

private:
    std::string name_;
    std::string alias_;
    ipme::utils::Json json_;
};
} // namespace ipme::wb::sage

#endif // DEFAULT_SAGE_MESSAGE_HANDLER_H
