#include "sage_element.h"

#include <iostream>
#include <sstream>

#include "utils/logger.h"

namespace ipme::wb::sage {
Sage_element::Sage_element(const std::string& id, const std::string& display_id,
                           double left, double top, double width, double height)
    : id_{id}, display_id_{display_id}
{
    update(left, top, width, height);
    INFO() << "CREATE [" << id << "]\n";
}

void Sage_element::update(double left, double top, double width, double height)
{
    // Some kind of unique/writer lock
    left_ = left;
    top_ = top;
    width_ = width;
    height_ = height;

    DEBUG() << "UPDATE [" << id_ << "] to L:" << left_ << " T: " << top_
            << " W:" << width_ << " H:" << height_;
}

std::string Sage_element::to_string() const
{
    std::stringstream ss;
    ss << "[" << id_ << "] to L:" << left_ << " T: " << top_ << " W:" << width_
       << " H:" << height_;
    return ss.str();
}

std::ostream& operator<<(std::ostream& out, const Sage_element& element)
{
    out << element.to_string();
    return out;
}

} // namespace ipme::wb::sage
