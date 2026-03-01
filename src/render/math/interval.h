#ifndef INTERVAL_H
#define INTERVAL_H

#include <stdbool.h>

typedef struct {
    double min;
    double max;
} Interval;

static inline Interval interval_from(double min, double max) {
    return (Interval){.min = min, .max = max};
}

bool contains(const Interval *interval, double x);
bool surrounds(const Interval *interval, double x);
bool overlaps(const Interval *a, const Interval *b);
double clamp(const Interval *interval, double x);

static inline Interval expand(const Interval interval, const double delta) {
    return (Interval){.min = interval.min - delta * 0.5,
                      .max = interval.max + delta * 0.5};
}

static inline Interval interval_combine(const Interval a, const Interval b) {
    return (Interval){
        .min = a.min < b.min ? a.min : b.min,
        .max = a.max > b.max ? a.max : b.max,
    };
}

#endif // INTERVAL_H
