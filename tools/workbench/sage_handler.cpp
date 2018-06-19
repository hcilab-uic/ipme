#include "sage_handler.h"

#include <boost/asio.hpp>

namespace ipme {
namespace wb {
Sage_handler::Sage_handler() : resolver_{ioc_}, wstream_{ioc_}
{
}

bool Sage_handler::connect(std::string_view host, std::string_view port)
{
    auto results = resolver_.resolve(host, port);
    auto connect = boost::asio::connect(wstream_.next_layer(), results.begin(),
                                        results.end());
    if(connect == results.end()) {
        throw std::runtime_error{"Connect failed"};
    }

    // throws system_error on failure
    wstream_.handshake(host.data(), "/");

    return true;
}
} // namespace wb
} // namespace ipme
