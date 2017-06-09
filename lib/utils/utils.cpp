#include "utils.h"

#include <cassert>
#include <cstring>
#include <ctime>

namespace ipme {
namespace utils {

std::string create_timestamp_string(const std::string& fmt)
{
    static constexpr size_t k_buffer_length = 128;
    assert(fmt.size() < k_buffer_length);

    char buffer[k_buffer_length];
    memset(buffer, 0, sizeof(buffer));
    std::time_t time = std::time(nullptr);
    std::strftime(buffer, sizeof(buffer), fmt.c_str(), std::localtime(&time));

    return buffer;
}

} // namespace utils
} // namespace ipme
