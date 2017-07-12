#include "trackable_object.h"

std::string ipme::data::Trackable_object::type_string(
    const ipme::data::Trackable_object::Type type)
{
    // FIXME: find a way to use switch statements
    if(Type::person == type) {
        return "person";
    }

    throw std::runtime_error("Unknown trackable object type");
}
