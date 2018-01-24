#ifndef IPME_WB_SCENE_OBJECT_H
#define IPME_WB_SCENE_OBJECT_H

#include <array>
#include <vector>

#include <QString>

#include "color.h"

namespace ipme::wb {
class Scene_object {
    static constexpr size_t point_dimension = 3;
    static constexpr size_t vector_dimenstion = 3;
    static constexpr size_t cells_per_object = 7;

public:
    Scene_object(const std::vector<QString>& parameters,
                 const QString& timestamp = "not-set");
    Scene_object(const std::vector<float>& parameters,
                 const QString& timestamp);

    virtual ~Scene_object() = default;

    static std::vector<Scene_object>
    create_from_csv(const QString& line, int ignore_cell_count = 2,
                    const QString& delimiter = ",");
    static std::vector<Scene_object>
    create_from_cells(const QStringList cells, int ignore_cell_count = 2);

    inline const std::array<float, point_dimension>& coords() const
    {
        return coords_;
    }

    inline const std::array<float, vector_dimenstion>& rot_vector() const
    {
        return rot_vector_;
    }

    inline float angle() const
    {
        return angle_;
    }

    inline const QString& timestamp() const
    {
        return timestamp_;
    }

    virtual void draw(bool show_centerline, bool show_tsegment) const;

private:
    static size_t next_index();

    size_t object_index_;
    Color color_;
    std::array<float, point_dimension> coords_;
    std::array<float, vector_dimenstion> rot_vector_;
    float angle_;
    QString timestamp_;

    static const std::vector<Color> object_colors;
};

} // namespace ipme::wb

#endif // IPME_WB_SCENE_OBJECT_H
