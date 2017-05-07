namespace cdi {
namespace data {
template <typename TCoordinate>
Point3D<TCoordinate>::Point3D(const TCoordinate x_, const TCoordinate y_,
                              const TCoordinate z_, const int raster_,
                              const double power_)
    : x{x_}, y{y_}, z{z_}, raster{raster_}, power{power_}
{
}
} // namespace data
} // namespace cdi
