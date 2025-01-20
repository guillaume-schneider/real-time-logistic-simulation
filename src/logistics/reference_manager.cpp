#include "reference_manager.hpp"
#include <stdexcept>

// Constructor
ReferenceManager::ReferenceManager(const std::string& dbFile) : m_db_file(dbFile) {
    loadDatabase();
}

void ReferenceManager::parseJson(json& database) {
    for (const auto& item : database["products"]) {
        m_products.push_back({
            item["name"].get<std::string>(),
            item["category"].get<std::string>(),
            item["sub_category"].get<std::string>(),
            item["year"].get<std::string>(),
            item["reference"].get<std::string>()
        });
    }

    for (const auto& [key, count] : database["counters"].items()) {
        m_counters[key] = count;
    }
}

void ReferenceManager::loadDatabase() {
    std::ifstream file(m_db_file);
    if (!file.is_open()) {
        std::cout << "Json Product Reference Error: " << m_db_file << " doesn't exist or can't be opened." << std::endl;
        return;
    }

    json database;
    try {
        file >> database;
        parseJson(database);
    } catch (const std::exception& e) {
        std::cout << "Error reading JSON: " << e.what() << std::endl;
    }
    file.close();
}

int ReferenceManager::getNextCounter(const std::string& category, const std::string& sub_category) {
    std::string key = category + "-" + sub_category;

    if (m_counters.find(key) == m_counters.end()) {
        m_counters[key] = 0;
    }

    m_counters[key] += 1;
    return m_counters[key];
}

std::string ReferenceManager::generateReference(const std::string& category, const std::string& sub_category, const std::string& year) {
    int counter = getNextCounter(category, sub_category);
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%s%s%s%04d", category.c_str(), sub_category.c_str(), year.c_str(), counter);
    return std::string(buffer);
}

// Public Methods
std::string ReferenceManager::addProduct(const std::string& name, const std::string& category, const std::string& sub_category, const std::string& year) {
    std::string reference = generateReference(category, sub_category, year);
    m_products.push_back({name, category, sub_category, year, reference});
    return reference;
}

ProductReference ReferenceManager::findProductByReference(const std::string& reference) {
    for (const auto& product : m_products) {
        if (product.getReference() == reference) {
            return product;
        }
    }
    return ProductReference{};
}

void ReferenceManager::saveToJson(const std::string& filename) {
    json jsonData;
    for (const auto& product : m_products) {
        jsonData["products"].push_back({
            {"name", product.getName()},
            {"category", product.getCategory()},
            {"sub_category", product.getSubCategory()},
            {"year", product.getYear()},
            {"reference", product.getReference()}
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

void ReferenceManager::loadFromJson(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        json jsonData;
        file >> jsonData;

        parseJson(jsonData);
        file.close();
    }
}

void ReferenceManager::displayProducts() const {
    for (const auto& product : m_products) {
        std::cout << "Name: " << product.getName() << "\n"
                  << "Category: " << product.getCategory() << "\n"
                  << "Sub-Category: " << product.getSubCategory() << "\n"
                  << "Year: " << product.getYear() << "\n"
                  << "Reference: " << product.getReference() << "\n"
                  << "-----------------------\n";
    }
}

ProductReference ReferenceManager::getProductByIndex(int index) {
    if (index >= m_products.size() || index < 0)
        throw std::out_of_range("Index out of range.");
    return m_products[index];
}
