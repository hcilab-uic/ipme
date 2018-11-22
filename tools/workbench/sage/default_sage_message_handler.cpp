#include "default_sage_message_handler.h"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "utils/json.h"
#include "utils/logger.h"

namespace ipme::wb::sage {
namespace pt = boost::property_tree;

void Default_sage_message_handler::handle_message(std::istream& in)
{
    json_.read(in);
    auto f_value = json_.get("f");

    if(f_value == "0000") {
    } else if(f_value == "000") {
    }
}

std::string Default_sage_message_handler::generate_registration_message()
{
    pt::ptree data_node;
    data_node.put("listener", name_);
    data_node.put("alias", alias_);

    pt::ptree root_node;
    root_node.put("f", "0000");
    root_node.add_child("d", data_node);

    std::ostringstream ss;
    pt::write_json(ss, root_node, false);

    return ss.str();
}

void Default_sage_message_handler::dispatch(const std::string& display_id,
                                            const utils::Json& message)
{
    // FIXME: Need to rethink this. Do we need separate classes?
    if(name_ == "setItemPositionAndSize" || name_ == "setItemPosition") {
        auto id = message.get("d.elemId");
        auto left = message.get<double>("d.elemLeft");
        auto top = message.get<double>("d.elemTop");
        auto width = message.get<double>("d.elemWidth");
        auto height = message.get<double>("d.elemHeight");
        container_->update_element(id, display_id, left, top, width, height);
    } else if(name_ == "deleteElement") {
        auto id = message.get("d.elemId");
        container_->delete_element(id);
        DEBUG() << message.to_string();
    } else {
        WARN() << "Unhandled " << name_ << "\n";
    }

    DEBUG() << "Dispatch called on " << name_ << " handler for processing";
}

void Default_sage_message_handler::set_element_container(
    std::shared_ptr<ipme::wb::sage::Sage_element_container> container)
{
    container_ = container;
    DEBUG() << "Setting element container for " << name_;
}

} // namespace ipme::wb::sage
