#include "similarity_finder.h"

#include <QDebug>

namespace ipme::wb {
Similarity_finder::Similarity_finder(
    const ipme::wb::Frame_collection& collection)
    : frames_{collection}
{
}

void Similarity_finder::on_find_similar(size_t begin, size_t end)
{
    qDebug() << "finding similarity to range " << begin << "-" << end << "\n";

    static constexpr size_t min_range_size = 100;
    static constexpr size_t buffer_length = 10;

    if(end - begin < min_range_size) {
        throw std::runtime_error{"range needs to be at least " +
                                 std::to_string(min_range_size) + " frames"};
    }

    [[maybe_unused]] int new_begin = begin + buffer_length;
    [[maybe_unused]] int new_end = end - buffer_length;
}

} // namespace ipme::wb
