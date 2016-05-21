#include <cmath>
#include <string>

#ifndef _HEAT_HPP_
#define _HEAT_HPP_

namespace heat {

static const double PI = 3.14159265359;

/* v^2 */
static inline size_t R(double v, size_t max = 0xff)
{
    if (std::isnan(v)) return max;
    if (v > 1) v = 1;
    if (v < 0) v = 0;
    return v * v * max;
};
/* sin(PI*v) */
static inline size_t G(double v, size_t max = 0xff)
{
    if (std::isnan(v)) return max;
    if (v > 1) v = 1;
    if (v < 0) v = 0;
    return sin(PI*v) * max;
};
/* (1-v)^2 */
static inline size_t B(double v, size_t max = 0xff)
{
    if (std::isnan(v)) return max;
    if (v > 1) v = 1;
    if (v < 0) v = 0;
    return (1 - v) * (1 - v) * max;
};

};

#endif /* Heat.hpp */
