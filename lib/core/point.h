#ifndef IPME_CORE_POINT_H
#define IPME_CORE_POINT_H

#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/geometries/point.hpp>

namespace ipme {
namespace core {
template <typename CoordinateType, size_t DimensionCount>
using Basic_point =
    boost::geometry::model::point<CoordinateType, DimensionCount,
                                  boost::geometry::cs::cartesian>;

// template <typename CoordinateType, std::size_t DimensionCount>
// using Point = Basic_point<CoordinateType, DimensionCount>;

template <typename CoordinateType, std::size_t DimensionCount>
class Point : public Basic_point<CoordinateType, DimensionCount> {
  public:
    using parent_type = Basic_point<CoordinateType, DimensionCount>;
    using coordinate_type = CoordinateType;
    static constexpr std::size_t dimension_count = DimensionCount;

    Point() = default;

    inline Point(coordinate_type v0, coordinate_type v1 = coordinate_type{},
                 coordinate_type v2 = coordinate_type{})
        : parent_type{v0, v1, v2}
    {
    }

    /// x coordinate
    inline coordinate_type x() const
    {
        return this->template get<0>();
    }

    /// y coordinate
    inline coordinate_type y() const
    {
        static_assert(dimension_count > 1, "y dimension is not defined");
        return this->template get<1>();
    }

    /// z coordinate
    inline coordinate_type z() const
    {
        static_assert(dimension_count > 2, "z dimension is not defined");
        return this->template get<2>();
    }
};

template <typename CoordinateType>
using Point2 = Point<CoordinateType, 2>;

template <typename CoordinateType>
using Point3 = Point<CoordinateType, 3>;

/// 2D Point (X, Y)
using Point2f = Point2<float>;
using Point2d = Point2<double>;

/// 3D Point (X, Y, Z)
using Point3f = Point3<float>;
using Point3d = Point3<double>;

} // namespace core
} // namespace ipme

namespace boost::geometry::traits {

template <typename CoordinateType, std::size_t DimensionCount>
struct tag<ipme::core::Point<CoordinateType, DimensionCount>> {
    using type = boost::geometry::point_tag;
};

template <typename CoordinateType, std::size_t DimensionCount>
struct coordinate_type<ipme::core::Point<CoordinateType, DimensionCount>> {
    using type = CoordinateType;
};

template <typename CoordinateType, std::size_t DimensionCount>
struct coordinate_system<ipme::core::Point<CoordinateType, DimensionCount>> {
    using type = boost::geometry::cs::cartesian;
};

template <typename CoordinateType, std::size_t DimensionCount>
struct dimension<ipme::core::Point<CoordinateType, DimensionCount>>
    : boost::mpl::int_<DimensionCount> {
};

template <typename CoordinateType, std::size_t DimensionCount,
          std::size_t Dimension>
struct access<ipme::core::Point<CoordinateType, DimensionCount>, Dimension> {
    static inline CoordinateType
    get(const ipme::core::Point<CoordinateType, DimensionCount>& p)
    {
        return p.template get<Dimension>();
    }

    static inline void set(ipme::core::Point<CoordinateType, DimensionCount>& p,
                           const CoordinateType& value)
    {
        p.template set<Dimension>(value);
    }
};
} // namespace boost::geometry::traits

#endif // IPME_CORE_POINT_H
