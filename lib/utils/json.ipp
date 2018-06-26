namespace ipme::utils {

template <typename ElementValue>
ElementValue Json::get(std::string_view path) const
{
    ElementValue value;
    try {
        value = ptree_.get<ElementValue>(path.data());
    } catch(const boost::property_tree::ptree_bad_path& ex) {
        std::cout << ex.what() << std::endl;
    }

    return value;
}
} // namespace ipme::utils
