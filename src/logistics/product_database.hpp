#ifndef PRODUCTDATABASE_HPP_
#define PRODUCTDATABASE_HPP_

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include "product.hpp"
#include <unordered_map>

class ProductDatabase {
private:
    std::vector<Product> m_products;

public:
    // Ajouter un produit à la base de données
    void addProduct(const Product& product) {
        auto it = std::find_if(m_products.begin(), m_products.end(),
            [&product](const Product& p) {
                return p.getSerialNumber() == product.getSerialNumber();
            });
        if (it != m_products.end()) {
            std::cerr << "Error: A product with the serial number '"
                      << product.getSerialNumber() << "' already exists.\n";
            return;
        }
        m_products.push_back(product);
    }

    // Supprimer un produit par numéro de série
    void removeProduct(const std::string& serialNumber) {
        auto it = std::remove_if(m_products.begin(), m_products.end(),
            [&serialNumber](const Product& p) {
                return p.getSerialNumber() == serialNumber;
            });
        if (it != m_products.end()) {
            m_products.erase(it, m_products.end());
            std::cout << "Product with serial number '" << serialNumber << "' removed.\n";
        } else {
            std::cerr << "Error: No product found with the serial number '" << serialNumber << "'.\n";
        }
    }

    // Rechercher un produit par numéro de série
    Product* findProduct(const std::string& serialNumber) {
        auto it = std::find_if(m_products.begin(), m_products.end(),
            [&serialNumber](const Product& p) {
                return p.getSerialNumber() == serialNumber;
            });
        if (it != m_products.end()) {
            return &(*it);
        }
        return nullptr; // Aucun produit trouvé
    }

    // Lister tous les produits
    void listProducts() const {
        if (m_products.empty()) {
            std::cout << "The product database is empty.\n";
            return;
        }

        for (const auto& product : m_products) {
            std::cout << "Serial Number: " << product.getSerialNumber() << "\n"
                      << "Product Reference: " << product.getProductReference().getName() << "\n"
                      << "Category: " << product.getProductReference().getCategory() << "\n"
                      << "Coordinates: (" << product.getCoordinates().x << ", "
                      << product.getCoordinates().y << ")\n\n";
        }
    }

    // Obtenir le nombre total de produits
    size_t getProductCount() const {
        return m_products.size();
    }

    // Lister les produits par catégorie
    void listProductsByCategory() const {
        if (m_products.empty()) {
            std::cout << "The product database is empty.\n";
            return;
        }

        std::unordered_map<std::string, int> categoryCount;
        for (const auto& product : m_products) {
            const std::string& category = product.getProductReference().getCategory();
            categoryCount[category]++;
        }

        std::cout << "Products by category:\n";
        for (const auto& [category, count] : categoryCount) {
            std::cout << "- " << category << ": " << count << " products\n";
        }
    }

    // Lister les produits par sous-catégorie
    void listProductsBySubCategory() const {
        if (m_products.empty()) {
            std::cout << "The product database is empty.\n";
            return;
        }

        std::unordered_map<std::string, int> subCategoryCount;
        for (const auto& product : m_products) {
            const std::string& subCategory = product.getProductReference().getSubCategory();
            subCategoryCount[subCategory]++;
        }

        std::cout << "Products by sub-category:\n";
        for (const auto& [subCategory, count] : subCategoryCount) {
            std::cout << "- " << subCategory << ": " << count << " products\n";
        }
    }
};

#endif // PRODUCTDATABASE_HPP_
