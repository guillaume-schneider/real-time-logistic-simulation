#include "coordinates.hpp"


double calculatePointDistance(const Point2D& a, const Point2D& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}
