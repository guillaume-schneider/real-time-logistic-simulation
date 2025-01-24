#ifndef PRODUCTDATABASE_HPP_
#define PRODUCTDATABASE_HPP_

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "product.hpp"
#include "../reference/reference_manager.hpp"
#include <unordered_map>
#include <memory>

class ProductDatabase {
private:
    std::unordered_map<std::string, std::vector<std::unique_ptr<Product>>> m_products;
    ReferenceManager* m_refManager = nullptr;
    std::string createSerialNumber(std::string reference);
    std::string extractProductReference(const std::string& serialNumber);
public:
    ProductDatabase() = default;
    ProductDatabase(const std::string& filename, ReferenceManager& refManager);
    bool init(const std::string& filename, ReferenceManager& refManager);
    void addProduct(std::unique_ptr<Product> product);
    void createProduct(const std::string& reference, int number);
    void removeProduct(const std::string& serialNumber);
    Product* findProduct(const std::string& serialNumber);
    void moveProduct(const std::string& serialNumber, ProductDatabase& destination);
    void listProducts() const;
    size_t getProductCount() const;
    void listProductsByCategory() const;
};

#endif // PRODUCTDATABASE_HPP_
