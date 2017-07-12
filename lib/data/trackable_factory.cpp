#include "trackable_factory.h"

#include "person.h"

namespace ipme {
namespace data {
namespace trackable_factory {

std::shared_ptr<Trackable_object> create(const std::string& type_desc,
                                         const std::string& id)
{
    if(type_desc == "person") {
        return std::make_shared<Person>(id);
    }

    throw std::runtime_error("unrecognized type: " + type_desc);
}

} // namespace trackable_factory
} // namespace data
} // namespace ipme
