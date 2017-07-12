#include "json_parser.h"

#include <boost/property_tree/json_parser.hpp>
#include <iostream>

namespace ipme {
namespace io {
Json_parser::Json_parser(const boost::filesystem::path& filename)
    : path_{filename}
{
}

boost::property_tree::ptree Json_parser::parse()
{
    boost::property_tree::json_parser::read_json(path_.string(), pt_);
    //    boost::property_tree::write_json(std::cout, pt_, true);
    return pt_;
}

} // namespace io
} // namespace ipme
