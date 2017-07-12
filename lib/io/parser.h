#ifndef IPME_IO_PARSER_H
#define IPME_IO_PARSER_H

#include <boost/property_tree/ptree.hpp>

namespace ipme {
namespace io {
class Parser {
  public:
    /**
     * virtual destructor
     */
    virtual ~Parser() = default;

    /**
     * @brief parse the main parser function
     */
    virtual boost::property_tree::ptree parse() = 0;
};
} // namespace io
} // namespace ipme

#endif // IPME_IO_PARSER_H
