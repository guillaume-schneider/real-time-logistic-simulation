#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include "coordinates.hpp"
#include "address.hpp"


class Location {
public:
    Address address;
    Point2D coordinates;

    Location() = default;
    Location(Address address, Point2D coordinnates) {
        this->address = address;
        this->coordinates = coordinates;
    }
    Location(const Location& other) {
        address = other.address;
        coordinates = other.coordinates;
    }
    ~Location() = default;

    Location& operator=(const Location& other) {
        if (this == &other) return *this;
        address = other.address;
        coordinates = other.coordinates;
        return *this;
    }

    double calculateDistance(const Location& other) {
        return calculatePointDistance(this->coordinates, other.coordinates);
    }
};

#endif
