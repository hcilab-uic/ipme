#ifndef IPME_WB_SAGE_SAGE_ELEMENT_H
#define IPME_WB_SAGE_SAGE_ELEMENT_H

#include "utils/json.h"

namespace ipme::wb::sage {
class Sage_element {
public:
    Sage_element(std::string_view id, double left, double top, double width,
                 double height);

    void update(double left, double top, double width, double height);

    inline std::string_view id() const
    {
        return id_;
    }

    inline double left() const
    {
        return left_;
    }

    inline double top() const
    {
        return top_;
    }

    inline double width() const
    {
        return width_;
    }

    inline double height() const
    {
        return height_;
    }

private:
    std::string id_;
    double left_ = 0.;
    double top_ = 0.;
    double width_ = 0.;
    double height_ = 0.;
};

} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_SAGE_ELEMENT_H
