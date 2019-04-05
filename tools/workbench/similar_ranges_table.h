#ifndef SIMILAR_RANGES_TABLE_H
#define SIMILAR_RANGES_TABLE_H

#include <QAbstractTableModel>
#include <memory>

#include <similarity_finder.h>

namespace ipme::wb {
class Similar_ranges_table : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit Similar_ranges_table(
        std::shared_ptr<Similarity_finder> similarity_finder,
        QObject* parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    inline void set_similarity_finder(std::shared_ptr<Similarity_finder> finder)
    {
        similarity_finder_ = finder;
    }

    void add_ranges(const Similarity_finder::Range_container& ranges);

private:
    std::shared_ptr<Similarity_finder> similarity_finder_;
    Similarity_finder::Range_container ranges_;
};

} // namespace ipme::wb
#endif // SIMILAR_RANGES_TABLE_H
