#include "sage_element.h"

#include <iostream>

namespace ipme::wb::sage {
Sage_element::Sage_element(std::string_view id, double left, double top,
                           double width, double height)
    : id_{id}
{
    update(left, top, width, height);
    std::cout << "CREATE [" << id << "]\n";
}

void Sage_element::update(double left, double top, double width, double height)
{
    // Some kind of unique/writer lock
    left_ = left;
    top_ = top;
    width_ = width;
    height_ = height;

    std::cout << "UPDATE [" << id_ << "] to L:" << left_ << " T: " << top_
              << " W:" << width_ << " H:" << height_ << "\n";
}

} // namespace ipme::wb::sage
