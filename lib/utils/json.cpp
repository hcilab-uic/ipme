#include "json.h"

#include <sstream>

namespace ipme {
namespace utils {
namespace pt = boost::property_tree;

void Json::read(const std::filesystem::__cxx11::path& path)
{
    std::ifstream in{path};
    read(in);
}

void Json::read(std::istream& in)
{
    pt::read_json(in, ptree_);
}

std::string Json::to_string() const
{
    std::ostringstream ss;
    pt::write_json(ss, ptree_);
    return ss.str();
}

} // namespace utils
} // namespace ipme
