#ifndef IPME_WB_SAGE_ELEMENT_CONTAINER_H
#define IPME_WB_SAGE_ELEMENT_CONTAINER_H

#include <string_view>
#include <unordered_map>

#include "sage_element.h"
#include "utils/json.h"

namespace ipme::wb::sage {
class Sage_element_container {
public:
    using Container_type = std::unordered_map<std::string, Sage_element>;

    void add_element(const ipme::utils::Json& message);
    void add_element(const std::string& id, double left, double top,
                     double width, double height);
    void update_element(const std::string& id, double left, double top,
                        double width, double height);
    void delete_element(const std::string& id);

    inline const Container_type& elements() const
    {
        // FIXME: Some kind of shared/reader lock on the entire container here
        return elements_;
    }

private:
    // This is monotonically increasing, does not decrease on delete
    size_t element_add_counter_ = 0;

    Container_type elements_;
};

} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_ELEMENT_CONTAINER_H
