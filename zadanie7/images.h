#ifndef IMGAES_H
#define IMGAES_H

#include <cmath>
#include <functional>

#include "color.h"
#include "coordinate.h"
#include "functional.h"

using Fraction = float;

template <typename T>
using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;

using Image = Base_image<Color>;

using Blend = Base_image<Fraction>;

namespace Detail {
inline Point toPolar(const Point p) {
    if (p.is_polar) return p;
    auto x = std::atan2(p.second, p.first);
    return Point(distance(p), x >= 0 ? x : (2 * M_PI + x), true);
}

inline Point fromPolar(const Point p) {
    if (!p.is_polar) return p;
    return Point(p.first * std::cos(p.second), p.first * std::sin(p.second), false);
}

}  // namespace Detail

template <typename T>
Base_image<T> constant(T t) {
    return [=](const Point) { return t; };
}

template <typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
    return compose(
        Detail::toPolar, [=](const Point p) { return Point(p.first, p.second - phi, true); }, image);
}

template <typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
    return compose(
        Detail::fromPolar, [=](const Point p) { return Point(p.first - v.first, p.second - v.second); }, image);
}

template <typename T>
Base_image<T> scale(Base_image<T> image, double s) {
    return compose(
        Detail::toPolar, [=](const Point p) { return Point(p.first / s, p.second, true); }, image);
}

template <typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
    return compose(Detail::fromPolar,
                   [=](const Point p) { return distance(p, Detail::fromPolar(q)) <= r ? inner : outer; });
}

template <typename T>
Base_image<T> checker(double d, T this_way, T that_way) {
    return compose(Detail::fromPolar, [=](const Point p) {
        return (int)(floor(p.first / d) + floor(p.second / d)) % 2 == 0 ? this_way : that_way;
    });
}

template <typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
    return compose(
        Detail::toPolar, [=](const Point p) { return Point(p.first, (d * n * p.second / (2 * M_PI)), false); },
        checker(d, this_way, that_way));
}

template <typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
    return compose(Detail::fromPolar, [=](const Point p) {
        return (int)(distance(p, Detail::fromPolar(q)) / d) % 2 == 0 ? this_way : that_way;
    });
}

template <typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
    return compose(Detail::fromPolar,
                   [=](const Point p) { return (-1 * d) / 2 <= p.first && p.first <= d / 2 ? this_way : that_way; });
}

inline Image cond(Region region, Image this_way, Image that_way) {
    return [=](const Point p) { return region(p) ? this_way(p) : that_way(p); };
}

inline Image lerp(Blend blend, Image this_way, Image that_way) {
    return [=](const Point p) { return this_way(p).weighted_mean(that_way(p), blend(p)); };
}

inline Image darken(Image image, Blend blend) {
    return [=](const Point p) { return image(p).weighted_mean(Colors::black, blend(p)); };
}

inline Image lighten(Image image, Blend blend) {
    return [=](const Point p) { return image(p).weighted_mean(Colors::white, blend(p)); };
}

#endif