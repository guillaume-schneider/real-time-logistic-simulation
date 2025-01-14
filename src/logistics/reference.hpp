#ifndef REFERENCE_HPP_
#define REFERENCE_HPP_

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <unordered_map>

using json = nlohmann::json;

struct ProductReference {
    std::string name;
    std::string category;
    std::string sub_category;
    std::string year;
    std::string reference;
};

class ReferenceManager {
private:
    std::string m_db_file;
    std::vector<ProductReference> m_products;
    std::unordered_map<std::string, int> m_counters;

    ProductReference parseJsonProduct(json jsonReference) {
        ProductReference result{};
        result.name = jsonReference["name"];
        result.category = jsonReference["category"];
        result.sub_category = jsonReference["sub_category"];
        result.year = jsonReference["year"];
        result.reference = jsonReference["reference"];
        return result;
    }

    void parseJson(json& database) {
        for (const auto& product : database["product"]) {
            m_products.push_back(parseJsonProduct(product));
        }

        for (const auto& counter : database["counters"]) {
            m_counters[counter] = database[counter];
        }
    }

    void loadDatabase() {
        std::ifstream file(m_db_file);
        json database;
        if (file.is_open()) {
            try {
                file >> database;
                parseJson(database);
            } catch (...) {
                std::cout << "Json Product Reference Error: " 
                    << "Can\'t open " << m_db_file << std::endl;
            }
            file.close();
        } else {
            std::cout << "Json Product Reference Error: " 
                << m_db_file << "doesn\'t exists." << std::endl;
        }
    }

    int getNextCounter(const std::string& category, const std::string& sub_category) {
        std::string key = category + "-" + sub_category;

        // Check if the key exists, if not initialize it to 0
        if (m_counters.find(key) == m_counters.end()) {
            m_counters[key] = 0;
        }

        m_counters[key] += 1;
        return m_counters[key];
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
        // json product = {
        //     {"name", name},
        //     {"category", category},
        //     {"sub_category", sub_category},
        //     {"year", year},
        //     {"reference", reference}
        // };
        // m_database["products"].push_back(product);
        m_products.push_back({name, category, sub_category, year, reference});
        return reference;
    }

    ProductReference findProductByReference(const std::string& reference) {
        for (const auto& product : m_products) {
            if (product.reference == reference) {
                return product;
            }
        }
        return ProductReference{};
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

        for (const auto& pair : m_counters) {
            jsonData["counters"][pair.first] = pair.second;
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

            for (const auto& [key, count] : jsonData["counters"].items()) {
                m_counters[key] = count;
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
