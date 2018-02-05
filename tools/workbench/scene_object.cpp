#include "scene_object.h"

#include <vector>

#include <QDebug>
#include <QStringList>

#include "color.h"
#include "core/point.h"
#include "geometry.h"

namespace ipme::wb {
const std::vector<Color> Scene_object::object_colors = {{1.f, 0.f, 0.f},
                                                        {0.f, 1.f, 0.f},
                                                        {0.f, 0.f, 1.f},
                                                        {1.f, 1.f, 0.f},
                                                        {1.f, 0.f, 1.f}};

static std::vector<float> make_floats(const std::vector<QString>& cells)
{
    std::vector<float> float_cells;
    for(const auto cell : cells) {
        float_cells.push_back(cell.toFloat());
    }

    return float_cells;
}

Scene_object::Scene_object(const std::vector<QString>& parameters,
                           const QString& timestamp)
    : Scene_object{make_floats(parameters), timestamp}
{
}

Scene_object::Scene_object(const std::vector<float>& parameters,
                           const QString& timestamp)
    : object_index_{next_index()},
      color_{object_colors[object_index_ % object_colors.size()]},
      coords_{{parameters[0], parameters[1], parameters[2]}},
      rot_vector_{{parameters[4], parameters[5], parameters[6]}},
      angle_{parameters[3]}, timestamp_{timestamp}
{
}

std::vector<Scene_object>
Scene_object::create_from_csv(const QString& line, int ignore_cell_count,
                              const QString& delimiter)
{
    const auto cells = line.split(delimiter);
    return create_from_cells(cells, ignore_cell_count);
}

std::vector<Scene_object>
Scene_object::create_from_cells(const QStringList cells, int ignore_cell_count)
{
    std::vector<QString> object_cells(cells_per_object);
    std::vector<Scene_object> objects;
    qInfo() << "Ignoring " << ignore_cell_count << " cells";
    ignore_cell_count = 2;
    for(int i = 0; i < cells.size(); ++i) {
        if(i < ignore_cell_count) {
            continue;
        }

        int object_cell_index = (i - ignore_cell_count) % cells_per_object;
        object_cells[object_cell_index] = cells[i];
        if(object_cell_index == (cells_per_object - 1) /* last object cell */) {
            QString timestamp{"not-set"};
            if(ignore_cell_count > 0) {
                timestamp = cells[0];
            }
            objects.emplace_back(object_cells, timestamp);
        }
    }

    return objects;
}

core::Polygon3f::ring_type Scene_object::create_polygon_ring(
    core::Polygon3f::point_type::coordinate_type x,
    core::Polygon3f::point_type::coordinate_type y,
    core::Polygon3f::point_type::coordinate_type z, float center_angle) const
{
    //    auto center_angle = angle() + pi;
    float half_segment_angle_radian = ts_angle_ * .5f;
    auto left_angle = center_angle - half_segment_angle_radian;
    auto right_angle = center_angle + half_segment_angle_radian;

    core::Point3f p0{x, y, z};
    core::Point3f p1{x + (max_length * std::cos(left_angle)),
                     y + (max_length * std::sin(left_angle)), z};
    core::Point3f p2{x + (max_length * std::cos(right_angle)),
                     y + (max_length * std::sin(right_angle)), z};

    return core::Polygon3f::ring_type{p0, p1, p2};
}

ipme::core::Polygon3f Scene_object::transaction_segment(float angle) const
{
    const auto c = coords();
    core::Polygon3f polygon{
        create_polygon_ring(c[0] / 3.f, c[2] / 3.f * -1.f, c[1] / 3.f, angle)};

    return polygon;
}

void Scene_object::draw(bool show_centerline, bool show_tsegment) const
{
    static constexpr float radius = 0.0125f;
    static const Color line_color{.15f, .15f, .15f, .1f};

    const auto c = coords();

    const core::Point3f p{c[0] / 3.f, c[2] / 3.f * -1.f, c[1] / 3.f};
    //    const Point p{c[0] / 3.f, c[1] / 3.f, c[2] / 3.f};

    auto effective_angle = angle() + pi;

    // Somewhat pessimistic view that if my co-ordinates are dead in the center,
    // then I must have invalid data
    if(p.x() == 0.f && p.y() == 0.f && p.z() == 0.f) {
        return;
    }

    // FIXME
    const auto index_in_component = object_index_ % object_colors.size();

    // FIXME: This is very ugly. Please subclass Scene_object and override draw
    if(index_in_component == 0) {
        // head
        Geometry::draw_circle(p, radius * 1.f, color_);
        if(show_tsegment) {
            //            Geometry::draw_segment(p, 2.f, effective_angle,
            //            color_);
            draw_transaction_segment(effective_angle);
        }
        if(show_centerline) {
            Geometry::draw_line(p, 2.f, effective_angle, line_color);
        }
    } else if(index_in_component == 1) {
        // torso
        Geometry::draw_square(p, radius * 1.5f, color_);
        if(show_tsegment) {
            draw_transaction_segment(effective_angle);
        }
        if(show_centerline) {
            Geometry::draw_line(p, 2.f, effective_angle, line_color);
        }
    } else if(index_in_component == 2 || index_in_component == 3) {
        // arms
        Geometry::draw_circle(p, radius * .5f, color_);
    } else {
        Geometry::draw_square(p, radius * 1.f, color_);

        // For device, the segment and the line are draws in the reverse
        // direction
        if(show_tsegment) {
            draw_transaction_segment(effective_angle + pi);
        }
        if(show_centerline) {
            Geometry::draw_line(p, .5f, effective_angle + pi, line_color);
        }
    }
}

void Scene_object::draw_transaction_segment(float angle) const
{
    Geometry::draw_polygon(transaction_segment(angle), color_);
}

void Scene_object::set_ts_angle(float angle)
{
    ts_angle_ = angle / 180.f * pi;
}

size_t Scene_object::next_index()
{
    static size_t running_index = 0;
    return running_index++;
    return running_index++;
}

} // namespace ipme::wb
