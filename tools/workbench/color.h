#ifndef IPME_WB_COLOR_H
#define IPME_WB_COLOR_H

#include <QString>

namespace ipme::wb {
struct Color {
    Color();

    Color(float r_, float g_, float b_, float a_ = 0.f);

    //    Color(const Color& other);
    //    Color(Color&& other);

    //    Color& operator=(const Color& other);
    //    Color& operator=(Color&& other);

    /// Return web hexadecimal format (eg: #FF0000 for (1.0, 0.0, 0.0))
    QString hex_string() const;

    QString stylesheet(const QString& fieldname) const;

    /// red
    float r = 0.f;

    /// green
    float g = 0.f;

    /// blue
    float b = 0.f;

    /// alpha
    float a = 0.f;
};
} // namespace ipme::wb

#endif // IPME_WB_COLOR_H
