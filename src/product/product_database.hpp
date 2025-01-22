#ifndef PRODUCTDATABASE_HPP_
#define PRODUCTDATABASE_HPP_

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "product.hpp"
#include "reference_manager.hpp"
#include <unordered_map>

class ProductDatabase {
private:
    std::unordered_map<std::string, std::vector<Product>> m_products;
    ReferenceManager* m_refManager = nullptr;
    std::string createSerialNumber(std::string reference);

public:
    ProductDatabase() = default;
    ProductDatabase(const std::string& filename, ReferenceManager& refManager);
    bool init(const std::string& filename, ReferenceManager& refManager);
    void addProduct(const Product& product);
    std::vector<Product> createProduct(const std::string& reference, int number);
    void removeProduct(const std::string& serialNumber);
    Product* findProduct(const std::string& serialNumber);
    void listProducts() const;
    size_t getProductCount() const;
    void listProductsByCategory() const;
};

#endif // PRODUCTDATABASE_HPP_
