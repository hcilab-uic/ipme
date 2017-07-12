#ifndef IPME_DATA_TRACKABLE_OBJECT_H
#define IPME_DATA_TRACKABLE_OBJECT_H

#include <memory>
#include <string>

namespace ipme {
namespace data {
class Trackable_object {
  public:
    enum class Type : uint8_t
    {
        person,
        count
    };

    /**
     * Virtual destructor
     */
    virtual ~Trackable_object() = default;

    /**
     * @brief id accessor function for an alpha-numeric unique identifier
     * @return object identifier (eg: person_12)
     */
    virtual std::string id() const = 0;

    /**
     * @brief type object type
     * @return type
     */
    virtual Type type() const = 0;

    /**
     * @brief to_string string representation of the trackable object
     * @return stringified Trackable_objectj
     */
    virtual std::string to_string() const = 0;

    /**
     * @brief type_string convenience function to print trackable object type
     * @param type enum
     * @return string representation
     */
    static std::string type_string(const Type type);
};
} // namespace data
} // namespace ipme

#endif // IPME_DATA_TRACKABLE_OBJECT_H
