#include "similar_ranges_table.h"

#include "utils/logger.h"

namespace ipme::wb {
Similar_ranges_table::Similar_ranges_table(
    std::shared_ptr<Similarity_finder> similarity_finder, QObject* parent)
    : QAbstractTableModel(parent), similarity_finder_{similarity_finder}
{
}

QVariant Similar_ranges_table::headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant{};
    }

    if(orientation == Qt::Orientation::Horizontal) {
        if(section > 1) {
            return QVariant{};
        }

        return section == 0 ? "Begin" : "End";
    }

    if(orientation == Qt::Orientation::Vertical) {
        return QString::number(section);
    }

    return QVariant{};
}

int Similar_ranges_table::rowCount(const QModelIndex& parent) const
{
    DEBUG() << "row_count " << ranges_.size();
    return ranges_.size();
}

int Similar_ranges_table::columnCount(const QModelIndex& parent) const
{
    DEBUG() << "col count 2";
    return 2;
}

QVariant Similar_ranges_table::data(const QModelIndex& index, int role) const
{
    DEBUG() << "data() called";
    int size = static_cast<int>(ranges_.size());

    if(index.row() >= size || index.column() > 1) {
        return QVariant();
    }

    if(role == Qt::DisplayRole) {
        auto row = ranges_[index.row()];
        return index.column() == 0 ? QString::number(row.first)
                                   : QString::number(row.second);
    }

    return QVariant();
}

void Similar_ranges_table::add_ranges(
    const Similarity_finder::Range_container& ranges)
{
    beginInsertRows(QModelIndex{}, 0, static_cast<int>(ranges.size()) - 1);
    ranges_ = ranges;
    endInsertRows();
}

} // namespace ipme::wb
