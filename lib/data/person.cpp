#include "person.h"

#include <sstream>

namespace ipme {
namespace data {

Person::Person(const std::string& id) : id_{id}
{
}

std::string ipme::data::Person::to_string() const
{
    std::stringstream ss;
    ss << "\t\t\t\tid: " << id_;
    ss << ", type: " << static_cast<uint32_t>(type_);
    // FIXME: add the remaining fields

    return ss.str();
}

} // namespace data
} // namespace ipme
