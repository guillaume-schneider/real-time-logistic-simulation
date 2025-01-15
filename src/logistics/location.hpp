#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <string>
#include <sstream>
#include <cmath>

struct Address {
    std::string street;  // Street address
    std::string city;    // City
    std::string zip;     // ZIP or postal code
    std::string country; // Country

    Address(const std::string& addrStreet, const std::string& addrCity, const std::string& addrZip, const std::string& addrCountry)
        : street(addrStreet), city(addrCity), zip(addrZip), country(addrCountry) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << street << ", " << city << " " << zip << ", " << country;
        return oss.str();
    }
};

struct Point2D {
    double x, y;
};

// Fonction pour calculer la distance entre deux points
double calculatePointDistance(const Point2D& a, const Point2D& b) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

class Location {
public:
    Address address;
    Point2D coordinates;

    double calculateDistance(const Location& other) {
        return calculatePointDistance(this->coordinates, other.coordinates);
    }
};

#endif
