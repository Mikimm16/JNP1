#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#include <functional>

inline auto compose() {
    return [](auto x) { return x; };
}

template <typename Arg, typename... Args>
auto compose(Arg arg, Args... args) {
    return [=](auto x) { return compose(args...)(arg(x)); };
}

template <typename H, typename... Fs>
auto lift(H h, Fs... fs) {
    return [=](auto... xs) { return h(fs(xs...)...); };
}

#endif