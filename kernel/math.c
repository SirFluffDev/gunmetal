#include "math.h"

f64 fabs(f64 x) {
    return x < 0.0 ? -x : x;
}

f64 sin(f64 x) {
    f64 result;
    asm("fsin" : "=t"(result) : "0"(x));
    return result;
}

f64 cos(f64 x) {
    return sin(x + PI / 2.0);
}