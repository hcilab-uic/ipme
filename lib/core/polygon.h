/* A tool for collecting, visualizing and annotating 3D Interaction data
 *
 * Copyright (C) 2017-2019 University of Illinois at Chicago
 *
 * Author: Harish G. Naik <hnaik2@uic.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
