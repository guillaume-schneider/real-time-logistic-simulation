#include "coordinates.hpp"


double calculatePointDistance(const Point2D& a, const Point2D& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}


int calculateTravelTimeInMs(const Point2D& a, const Point2D& b, const float& speed) {
    if (speed <= 0) {
        throw std::invalid_argument("Speed must be greater than zero");
    }
    double distance = calculatePointDistance(a, b);
    return static_cast<int>((distance / speed) * 3600. * 1000.);
}
