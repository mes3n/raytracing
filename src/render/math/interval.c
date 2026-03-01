#include "interval.h"

bool contains(const Interval *interval, double x) {
    return interval->min <= x && x <= interval->max;
}

bool surrounds(const Interval *interval, double x) {
    return interval->min < x && x < interval->max;
}

bool overlaps(const Interval *a, const Interval *b) {
    return surrounds(a, b->min) || surrounds(a, b->max);
}

double clamp(const Interval *interval, double x) {
    if (x < interval->min)
        return interval->min;
    if (x > interval->max)
        return interval->max;
    return x;
}
