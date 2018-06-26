#include "json.h"

namespace ipme {
namespace utils {
namespace pt = boost::property_tree;

void Json::read(std::istream& in)
{
    pt::read_json(in, ptree_);
}

} // namespace utils
} // namespace ipme
