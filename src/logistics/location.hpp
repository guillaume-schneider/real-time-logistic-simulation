#ifndef LOCATION_HPP_
#define LOCATION_HPP_

#include <string>
#include <sstream>

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

class Location {
public:
    Address address;
};

#endif
