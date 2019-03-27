#ifndef IPME_WB_SIMILARITY_FINDER_H
#define IPME_WB_SIMILARITY_FINDER_H

#include <vector>

#include <QObject>

#include "frame_collection.h"

namespace ipme::wb {
class Similarity_finder : public QObject {
    Q_OBJECT;

public:
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
    const ipme::wb::Frame_collection& frames_;
    std::vector<std::pair<size_t, size_t>> similar_ranges_;
};

} // namespace ipme::wb
#endif // IPME_WB_SIMILARITY_FINDER_H
