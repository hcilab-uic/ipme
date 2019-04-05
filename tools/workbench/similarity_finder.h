#ifndef IPME_WB_SIMILARITY_FINDER_H
#define IPME_WB_SIMILARITY_FINDER_H

#include <QObject>
#include <dlib/dnn.h>
#include <tuple>
#include <vector>

#include "frame_collection.h"

namespace ipme::wb {
class Similarity_finder {
public:
    using Row_type = dlib::matrix<double, 1, 1>;
    using Matrix_type = std::vector<Row_type>;
    using Label_vector = std::vector<unsigned long>;
    using Range_container = std::vector<std::pair<int, int>>;

    Similarity_finder(const ipme::wb::Frame_collection& collection);

    inline const Range_container& similar_ranges() const
    {
        return similar_ranges_;
    }

    void find_similar(size_t begin, size_t end);

private:
    Row_type generate_row(const Frame& frame);

    std::tuple<Matrix_type, Label_vector, Matrix_type>
    split_data(size_t range_begin, size_t range_end);

    const Frame_collection& frames_;
    Range_container similar_ranges_;
};

} // namespace ipme::wb
#endif // IPME_WB_SIMILARITY_FINDER_H
