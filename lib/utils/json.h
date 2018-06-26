#ifndef IPME_UTILS_JSON_H
#define IPME_UTILS_JSON_H

#include <istream>
#include <ostream>
#include <string_view>
#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace ipme {
namespace utils {
class Json
{
public:
    void read(std::istream& in);

    template<typename ElementValue = std::string>
    ElementValue get(std::string_view path) {
        ElementValue value;
        try {
            value = ptree_.get<ElementValue>(path.data());
            last_read_ = true;
        } catch (const boost::property_tree::ptree_bad_path& ex) {
            std::cout << ex.what() << std::endl;
            last_read_ = false;
        }

        return value;
    }

    bool last_read_success() const {
        return last_read_;
    }
private:
    bool last_read_ = false;
    boost::property_tree::ptree ptree_;
};

//template<>
//std::string Json::get(std::string_view path) {
//    std::string ret;
//    try {
//        ret = ptree_.get<std::string>(path.data());
//        last_read_ = true;
//    } catch(const boost::property_tree::ptree_bad_path& ex) {
//        std::cout << ex.what() << std::endl;
//        last_read_ = false;
//    }

//    return ret;
//}

} // namespace utils
} // namespace ipme

#include "json.ipp"

#endif // IPME_UTILS_JSON_H
