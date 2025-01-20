#ifndef COORDINATES_HPP_
#define COORDINATES_HPP_

#include <cmath>

struct Point2D {
    double x, y;
    Point2D() : x(0), y(0) {}
    Point2D(double x, double y) : x(x), y(y) {}
};

double calculatePointDistance(const Point2D& a, const Point2D& b);

#endif
