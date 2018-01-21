#include "frame_model.h"

namespace ipme::wb {
Frame_model::Frame_model()
{
}

QVariant Frame_model::data(const QModelIndex& index, int role) const
{
    if(role != Qt::DisplayRole) {
        return QVariant{};
    }

    const size_t r = static_cast<size_t>(index.row());
    const size_t c = static_cast<size_t>(index.column());

    if(r < data_.size() && c < data_[0].size()) {
        return data_[r][c];
    }

    return QVariant{};
}
} // namespace ipme::wb
