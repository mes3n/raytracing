#include "interval.h"

int contains(const Interval *interval, double x) {
    return interval->min <= x && x <= interval->max;
}

int surrounds(const Interval *interval, double x) {
    return interval->min < x && x < interval->max;
}

double clamp(const Interval *interval, double x) {
    if (x < interval->min)
        return interval->min;
    if (x > interval->max)
        return interval->max;
    return x;
}
