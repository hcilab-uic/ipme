namespace cdi {
namespace utils {
template <typename T>
Random<T>::Random(const T begin, const T end)
    : random_engine_{std::random_device()()}, distribution_{begin, end}
{
}

} // namespace utils
} // namespace cdi
