#ifndef IPME_DATA_TRACKABLE_FACTORY_H
#define IPME_DATA_TRACKABLE_FACTORY_H

#include <memory>

#include "trackable_object.h"

namespace ipme {
namespace data {
namespace trackable_factory {
std::shared_ptr<Trackable_object> create(const std::string& type_desc,
                                         const std::string& id);
} // namespace trackable_factory
} // namespace data
} // namespace ipme

#endif // IPME_DATA_TRACKABLE_FACTORY_H
