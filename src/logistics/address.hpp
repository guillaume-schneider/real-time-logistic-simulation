#ifndef ADDRESS_HPP_
#define ADDRESS_HPP_

#include <sstream>
#include <string>
#include <regex>
#include <random>


const std::regex pattern(R"((.*), ([^,]+) (\d{5}), (.+))");


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

    static Address generateRandomAddress() {
        std::vector<std::string> streets = {"Elm Street", "Maple Avenue", "Pine Road", "Oak Drive"};
        std::vector<std::string> cities = {"Springfield", "Rivertown", "Hill Valley", "Smallville"};
        std::vector<std::string> countries = {"USA", "Canada", "France", "Germany"};

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist_street(0, static_cast<int>(streets.size() - 1));
        std::uniform_int_distribution<int> dist_city(0, static_cast<int>(cities.size() - 1));
        std::uniform_int_distribution<int> dist_country(0, static_cast<int>(countries.size() - 1));
        std::uniform_int_distribution<> dist_zip(10000, 99999);

        std::string street = streets[dist_street(gen)];
        std::string city = cities[dist_city(gen)];
        std::string zip = std::to_string(dist_zip(gen));
        std::string country = countries[dist_country(gen)];

        return Address(street, city, zip, country);
    }

    static bool validateAddress(const std::string& address) {
        return std::regex_match(address, pattern);
    }

    static Address stringToAddress(const std::string& addressString) {
        std::smatch match;

        if (std::regex_match(addressString, match, pattern)) {
            std::string street = match[1].str();
            std::string city = match[2].str();
            std::string zip = match[3].str();
            std::string country = match[4].str();

            return Address(street, city, zip, country);
        } else {
            throw std::invalid_argument("Invalid address format: " + addressString);
        }
    }
};

#endif
