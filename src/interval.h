#ifndef INTERVAL_H
#define INTERVAL_H

typedef struct {
    double min;
    double max;
} Interval;

int interval_contains(const Interval* interval, double x);
int interval_surrounds(const Interval* interval, double x);

#endif // INTERVAL_H
