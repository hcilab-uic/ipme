#ifndef IPME_IO_JSON_PARSER_H
#define IPME_IO_JSON_PARSER_H

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>

#include "parser.h"

namespace ipme {
namespace io {
using ptree = boost::property_tree::ptree;

class Json_parser : public Parser {
  public:
    /**
     * @brief Json_parser constructor taking path of the file to be parsed
     * @param filename name of the file
     */
    Json_parser(const boost::filesystem::path& filename);

    /** @copydoc Parser::parse()
     */
    virtual boost::property_tree::ptree parse() override;

  private:
    ptree pt_;
    boost::filesystem::path path_;
};

} // namespace io
} // namespace ipme
#endif // IPME_IO_JSON_PARSER_H
