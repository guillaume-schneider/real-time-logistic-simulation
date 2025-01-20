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
    ProductDatabase() = default;
    std::unordered_map<std::string, std::vector<Product>> m_products;
    ReferenceManager* m_refManager = nullptr;
    std::string createSerialNumber(std::string reference);

public:
    static ProductDatabase& getInstance() {
        static ProductDatabase instance;
        return instance;
    }
    ProductDatabase(const ProductDatabase&) = delete;
    ProductDatabase& operator=(const ProductDatabase&) = delete;
    void addProduct(const Product& product);
    bool init(const std::string& filename, ReferenceManager& refManager);
    std::vector<Product> createProduct(const std::string& reference, int number);
    void removeProduct(const std::string& serialNumber);
    Product* findProduct(const std::string& serialNumber);
    void listProducts() const;
    size_t getProductCount() const;
    void listProductsByCategory() const;
};

#endif // PRODUCTDATABASE_HPP_
