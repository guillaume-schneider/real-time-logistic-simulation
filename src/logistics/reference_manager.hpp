#ifndef REFERENCE_HPP_
#define REFERENCE_HPP_

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include "product_reference.hpp"

using json = nlohmann::json;

class ReferenceManager {
private:
    std::string m_db_file;
    std::vector<ProductReference> m_products;
    std::unordered_map<std::string, int> m_counters;

    void parseJson(json& database);
    void loadDatabase();
    int getNextCounter(const std::string& category, const std::string& sub_category);
    std::string generateReference(const std::string& category, const std::string& sub_category, const std::string& year);

public:
    ReferenceManager() = default;
    ReferenceManager(const std::string& dbFile);

    std::string addProduct(const std::string& name, const std::string& category, const std::string& sub_category, const std::string& year);
    ProductReference findProductByReference(const std::string& reference);
    void saveToJson(const std::string& filename);
    void loadFromJson(const std::string& filename);
    void displayProducts() const;
    ProductReference getProductByIndex(int index);
};

#endif // REFERENCE_HPP_
