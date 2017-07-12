#ifndef IPME_DATA_PERSON_H
#define IPME_DATA_PERSON_H

#include <boost/optional.hpp>

#include "trackable_object.h"

namespace ipme {
namespace data {
class Person : public Trackable_object {
  public:
    /**
     * @brief Person constructor taking ID
     * @param id unique identifier assigned to the person
     */
    Person(const std::string& id);

    /** @copydoc Trackable_object::id()
     */
    inline virtual std::string id() const override
    {
        return id_;
    }

    /** @copydoc Trackable_object::type
     */
    inline virtual Type type() const override
    {
        return type_;
    }

    /** @copydoc Trackable_object::to_string() const
     */
    std::string to_string() const override;

  private:
    std::string id_;
    Trackable_object::Type type_ = Trackable_object::Type::person;
};
} // namespace data
} // namespace ipme

#endif // IPME_DATA_PERSON_H
