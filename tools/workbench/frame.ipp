namespace ipme::wb {
template <typename Container>
bool Frame::has_all_vrpn_ids(Container&& ids)
{
    for(const auto& id : ids) {
        if(vrpn_ids_.find(id) == std::end(vrpn_ids_)) {
            return false;
        }
    }

    return true;
}
} // namespace ipme::wb
