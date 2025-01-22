#ifndef ADDRESS_DATABASE_HPP_
#define ADDRESS_DATABASE_HPP_

#include <nlohmann/json.hpp>
#include "coordinates.hpp"
#include <functional>
#include <fstream>
#include <iostream>
#include "address.hpp"
#include <optional>
#include <regex>

class AddressDatabase {
private:
    nlohmann::json m_database;
    std::string m_filename;

    AddressDatabase() = default;

    Address parseAddress(const std::string& addressStr) const {
        std::regex pattern(R"((.*), ([^,]+) (\d+), (.+))");
        std::smatch match;

        if (std::regex_match(addressStr, match, pattern) && match.size() == 5) {
            return Address(
                match[1].str(),  // Street
                match[2].str(),  // City
                match[3].str(),  // ZIP
                match[4].str()   // Country
            );
        } else {
            throw std::invalid_argument("Invalid address format");
        }
    }

    std::string generateId(const Address& address) const {
        std::hash<std::string> hashFn;
        std::string concatenated = address.street + address.city + address.zip + address.country;
        return "ID" + std::to_string(hashFn(concatenated));
    }

    void saveDatabase() const {
        std::ofstream outFile(m_filename);
        outFile << m_database.dump(4);
        outFile.close();
    }

public:
    static AddressDatabase& getInstance() {
        static AddressDatabase instance;
        return instance;
    }
    AddressDatabase(const AddressDatabase&) = delete;
    AddressDatabase& operator=(const AddressDatabase&) = delete;

    void loadDatabase(const std::string& filename) {
        m_filename = filename;
        std::ifstream inFile(m_filename);
        if (inFile) {
            inFile >> m_database;
            inFile.close();
        }
    }

    void addAddress(const Address& address, double x, double y) {
        std::string id = generateId(address);

        m_database["by_id"][id] = {
            {"address", address.toString()},
            {"coordinates", {{"x", x}, {"y", y}}}
        };

        m_database["by_address"][address.toString()] = id;

        saveDatabase();
    }

    std::optional<Address> findById(const std::string& id) const {
        if (!m_database["by_id"].contains(id)) {
            std::cerr << "Address Database: Can\'t find \'" << id << "\'.\n";
            return std::nullopt;
        }

        return parseAddress(id);
    }

    std::optional<Point2D> findCoordinates(const Address& address) {
        auto addressId = generateId(address);
        if (!m_database["by_id"].contains(addressId)) {
            std::cerr << "Address Database: Can\'t find \'" << addressId << "\'.\n";
            return std::nullopt;
        }

        nlohmann::json coordinates = m_database["by_id"][addressId]["coordinates"];
        return Point2D{coordinates["x"], coordinates["y"]};
    }

    std::optional<std::string> findByAddress(const Address& address) const {
        std::string addressStr = address.toString();
        if (m_database["by_address"].contains(addressStr)) {
            return m_database["my_address"][addressStr];
        } else {
            std::cerr << "Address Database: Can\'t find \'" << addressStr << "\'.\n";
            return std::nullopt;
        }
    }

    void printAllEntries() const {
        if (m_database["by_id"].empty()) {
            std::cout << "Address Database: No entries found.\n";
            return;
        }

        for (const auto& [id, entry] : m_database["by_id"].items()) {
            std::cout << "ID: " << id << "\n";
            std::cout << "  Address: " << entry["address"] << "\n";
            std::cout << "  Coordinates: (" << entry["coordinates"]["x"]
                      << ", " << entry["coordinates"]["y"] << ")\n";
        }
    }
};

#endif
