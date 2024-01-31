#ifndef INTERVAL_H
#define INTERVAL_H

typedef struct {
    double min;
    double max;
} Interval;

static inline Interval interval_from(double min, double max) {
    return (Interval){.min = min, .max = max};
}

int contains(const Interval *interval, double x);
int surrounds(const Interval *interval, double x);
double clamp(const Interval *interval, double x);

#endif // INTERVAL_H
