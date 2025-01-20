#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <string>
#include <sstream>
#include "coordinates.hpp"

struct Address {
    std::string street;  // Street address
    std::string city;    // City
    std::string zip;     // ZIP or postal code
    std::string country; // Country

    Address() : street(""), city(""), zip(""), country("") {}
    Address(const std::string& addrStreet, const std::string& addrCity,
                const std::string& addrZip, const std::string& addrCountry)
        : street(addrStreet), city(addrCity), zip(addrZip), country(addrCountry) {}
    Address(const Address& other)
        : street(other.street), city(other.city), zip(other.zip), 
            country(other.country) {} 
    ~Address() = default;

    Address& operator=(const Address& other) {
        if (&other == this) return *this;
        street = other.street;
        city = other.city;
        zip = other.zip;
        country = other.country;
        return *this;
    }

    std::string toString() const {
        std::ostringstream oss;
        oss << street << ", " << city << " " << zip << ", " << country;
        return oss.str();
    }
};

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
