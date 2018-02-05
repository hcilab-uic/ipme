#ifndef IPME_CORE_POLYGON_H
#define IPME_CORE_POLYGON_H

#include <deque>

#include <boost/geometry.hpp>

#include "point.h"

namespace ipme {
namespace core {
template <typename Point>
using Polygon = boost::geometry::model::polygon<Point>;

using Polygon2f = Polygon<Point2f>;
using Polygon2d = Polygon<Point2d>;

using Polygon3f = Polygon<Point3f>;
using Polygon3d = Polygon<Point3d>;

template <typename Polygon>
std::deque<Polygon> compute_intersection(const Polygon& p0, const Polygon& p1)
{
    std::deque<Polygon> outputs;
    boost::geometry::intersection(p0, p1, outputs);
    return outputs;
}

template <typename PolygonContainer>
std::deque<typename PolygonContainer::value_type>
compute_intersection(const PolygonContainer& polygons)
{
    assert(polygons.size() > 1);

    return compute_intersection(polygons[0], polygons[1]);
}

template <typename Container>
Polygon3f construct_polygon(const Container& points)
{
    return Polygon3f{
        Polygon3f::ring_type{std::begin(points), std::end(points)}};
}
} // namespace core
} // namespace ipme

#endif // IPME_CORE_POLYGON_H
