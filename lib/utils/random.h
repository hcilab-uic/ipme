/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CDI_LIB_UTILS_RANDOM_H
#define CDI_LIB_UTILS_RANDOM_H

#include <random>
#include <type_traits>

namespace ipme {
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
} // namespace ipme

#include "random.ipp"

#endif // CDI_LIB_UTILS_RANDOM_H
