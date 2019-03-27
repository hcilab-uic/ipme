#ifndef IPME_WB_SIMILARITY_FINDER_H
#define IPME_WB_SIMILARITY_FINDER_H

#include <QObject>
#include <dlib/dnn.h>
#include <vector>

#include "frame_collection.h"

namespace ipme::wb {
class Similarity_finder : public QObject {
    Q_OBJECT;

public:
    using Input_matrix = std::vector<dlib::matrix<double, 0, 1>>;
    using Label_vector = std::vector<unsigned long>;
    using Range_container = std::vector<std::pair<size_t, size_t>>;

    Similarity_finder(const ipme::wb::Frame_collection& collection);

    inline const Range_container& similar_ranges() const
    {
        return similar_ranges_;
    }

public slots:
    void on_find_similar(size_t begin, size_t end);

signals:
    void finished_similarity_finding();

private:
    Input_matrix generate_training_data(size_t range_begin, size_t range_end);

    const ipme::wb::Frame_collection& frames_;
    std::vector<std::pair<size_t, size_t>> similar_ranges_;
};

} // namespace ipme::wb
#endif // IPME_WB_SIMILARITY_FINDER_H
