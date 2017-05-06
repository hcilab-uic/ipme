#ifndef CDI_LIB_UTILS_RANDOM_H
#define CDI_LIB_UTILS_RANDOM_H

#include <random>
#include <type_traits>

namespace cdi {
namespace utils {
template <typename T>
class Random {
  public:
    /**
     * @brief Constructor taking range
     * @param begin range
     * @param end range
     */
    Random(const T begin, const T end);

    /**
     * @brief next get the next random number
     * @return random number in the range
     */
    inline T next()
    {
        return distribution_(random_engine_);
    }

  private:
    std::mt19937 random_engine_;

    using Distribution_t =
        typename std::conditional<std::is_integral<T>::value,
                         std::uniform_int_distribution<T>,
                         std::uniform_real_distribution<T>>::type;
    Distribution_t distribution_;
};
} // namespace utils
} // namespace cdi

#include "random.ipp"

#endif // CDI_LIB_UTILS_RANDOM_H
