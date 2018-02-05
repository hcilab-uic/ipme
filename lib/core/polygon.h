#ifndef IPME_CORE_POLYGON_H
#define IPME_CORE_POLYGON_H

#include <boost/geometry/geometries/polygon.hpp>

#include "point.h"

namespace ipme {
namespace core {
template <typename Point>
using Polygon = boost::geometry::model::polygon<Point>;

using Polygon2f = Polygon<Point2f>;
using Polygon2d = Polygon<Point2d>;

using Polygon3f = Polygon<Point3f>;
using Polygon3d = Polygon<Point3d>;

} // namespace core
} // namespace ipme

#endif // IPME_CORE_POLYGON_H
