#ifndef IPME_WB_SAGE_SAGE_ELEMENT_H
#define IPME_WB_SAGE_SAGE_ELEMENT_H

#include <ostream>

#include "utils/json.h"

namespace ipme::wb::sage {
class Sage_element {
public:
    Sage_element(const std::string& id, const std::string& display_id,
                 double left, double top, double width, double height);

    void update(double left, double top, double width, double height);

    inline const std::string& id() const
    {
        return id_;
    }

    inline const std::string& display_id() const
    {
        return display_id_;
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

    std::string to_string() const;

private:
    std::string id_;
    std::string display_id_;
    double left_ = 0.;
    double top_ = 0.;
    double width_ = 0.;
    double height_ = 0.;
};

std::ostream& operator<<(std::ostream& out, const Sage_element& element);
} // namespace ipme::wb::sage

#endif // IPME_WB_SAGE_SAGE_ELEMENT_H
