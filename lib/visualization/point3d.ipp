namespace cdi {
namespace visualization {
template <typename TCoordinate>
Point3D<TCoordinate>::Point3D(const TCoordinate x_, const TCoordinate y_,
                              const TCoordinate z_)
    : x{x_}, y{y_}, z{z_}
{
}
} // namespace visualization
} // namespace cdi
