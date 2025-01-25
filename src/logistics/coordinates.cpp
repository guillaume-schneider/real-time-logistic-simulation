#include "coordinates.hpp"


double calculatePointDistance(const Point2D& a, const Point2D& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}


float calculateTravelTimeInSeconds(const Point2D& a, const Point2D& b, const float& speed) {
    if (speed <= 0) {
        throw std::invalid_argument("Speed must be greater than zero");
    }
    float distance = calculatePointDistance(a, b);
    return (distance / speed) * 3600.0f;
}
