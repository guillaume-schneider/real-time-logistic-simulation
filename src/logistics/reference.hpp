#ifndef REFERENCE_HPP_
#define REFERENCE_HPP_

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using json = nlohmann::json;

struct Product {
    std::string name;
    std::string category;
    std::string sub_category;
    std::string year;
    std::string reference;
};

class ReferenceManager {
private:
    std::string m_db_file;
    json m_database;
    std::vector<Product> m_products;

    void loadDatabase() {
        std::ifstream file(m_db_file);
        if (file.is_open()) {
            try {
                file >> m_database;
            } catch (...) {
                m_database["products"] = json::array();
                m_database["counters"] = json::object();
            }
            file.close();
        } else {
            m_database["products"] = json::array();
            m_database["counters"] = json::object();
        }
    }

    int getNextCounter(const std::string& category, const std::string& sub_category) {
        std::string key = category + "-" + sub_category;

        // Check if the key exists, if not initialize it to 0
        if (m_database["counters"].find(key) == m_database["counters"].end()) {
            m_database["counters"][key] = 0;
        }

        // Safely increment the counter
        int counter = m_database["counters"][key].get<int>();
        m_database["counters"][key] = counter + 1;

        return counter + 1;
    }

    std::string generateReference(const std::string& category, const std::string& sub_category, const std::string& year) {
        int counter = getNextCounter(category, sub_category);
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%s%s%s%04d", category.c_str(), sub_category.c_str(), year.c_str(), counter);
        return std::string(buffer);
    }

public:
    ReferenceManager() = default;

    ReferenceManager(const std::string& dbFile) : m_db_file(dbFile) {
        loadDatabase();
    }

    std::string addProduct(const std::string& name, const std::string& category, const std::string& sub_category, const std::string& year) {
        std::string reference = generateReference(category, sub_category, year);
        json product = {
            {"name", name},
            {"category", category},
            {"sub_category", sub_category},
            {"year", year},
            {"reference", reference}
        };
        m_database["products"].push_back(product);
        m_products.push_back({name, category, sub_category, year, reference});
        return reference;
    }

    json findProductByReference(const std::string& reference) {
        for (const auto& product : m_database["products"]) {
            if (product["reference"] == reference) {
                return product;
            }
        }
        return nullptr;
    }

    void saveToJson(const std::string& filename) {
        json jsonData;
        for (const auto& product : m_products) {
            jsonData["products"].push_back({
                {"name", product.name},
                {"category", product.category},
                {"sub_category", product.sub_category},
                {"year", product.year},
                {"reference", product.reference}
            });
        }

        std::ofstream file(filename);
        if (file.is_open()) {
            file << std::setw(4) << jsonData; // Pretty-print with 4 spaces
            file.close();
            std::cout << "Product references saved to " << filename << std::endl;
        } else {
            std::cerr << "Unable to open file: " << filename << std::endl;
        }
    }

    void loadFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            json jsonData;
            file >> jsonData;

            for (const auto& item : jsonData["products"]) {
                m_products.push_back({
                    item["name"].get<std::string>(),
                    item["category"].get<std::string>(),
                    item["sub_category"].get<std::string>(),
                    item["year"].get<std::string>(),
                    item["reference"].get<std::string>()
                });
            }
            file.close();
        }
    }

    void displayProducts() const {
        for (const auto& product : m_products) {
            std::cout << "Name: " << product.name << "\n"
                      << "Category: " << product.category << "\n"
                      << "Sub-Category: " << product.sub_category << "\n"
                      << "Year: " << product.year << "\n"
                      << "Reference: " << product.reference << "\n"
                      << "-----------------------\n";
        }
    }
};


#endif
