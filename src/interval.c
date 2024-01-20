#include "interval.h"

int interval_contains(const Interval* interval, double x) {
    return interval->min <= x && x <= interval->max;
}
int interval_surrounds(const Interval* interval, double x) {
    return interval->min < x && x < interval->max;
}
