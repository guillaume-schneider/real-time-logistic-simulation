#ifndef COORDINATES_HPP_
#define COORDINATES_HPP_

#include <cmath>
#include <iostream>

struct Point2D {
    double x, y;
    Point2D() : x(0), y(0) {}
    Point2D(double x, double y) : x(x), y(y) {}

    friend std::ostream& operator<<(std::ostream& os, const Point2D& point) {
        os << "(" << point.x << ", " << point.y << ")";
        return os;
    }
};

double calculatePointDistance(const Point2D& a, const Point2D& b);
int calculateTravelTimeInMs(const Point2D& a, const Point2D& b, const float& speed); 

#endif
