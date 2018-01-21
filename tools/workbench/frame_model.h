#ifndef IPME_WB_FRAME_MODEL_H
#define IPME_WB_FRAME_MODEL_H

#include <QAbstractItemModel>

namespace ipme::wb {
class Frame_model : public QAbstractItemModel {
  public:
    Frame_model();

    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const override;

  private:
    std::vector<std::vector<float>> data_{{10.f, 20.f}, {20.f, 30.f}};
};
} // namespace ipme::wb

#endif // IPME_WB_FRAME_MODEL_H
