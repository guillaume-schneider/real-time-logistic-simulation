#include "product_database.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../../logistics/coordinates.hpp"
#include <regex>

using json = nlohmann::json;

constexpr size_t categoryLength = 4;       
constexpr size_t subCategoryLength = 3;    
constexpr size_t yearLength = 2;

std::string ProductDatabase::extractProductReference(const std::string& serialNumber) {
    // Regex pour valider et extraire la référence produit
    std::regex serialRegex("([A-Z]+\\d+\\d{2})(\\d{4})");
    std::smatch match;

    if (std::regex_match(serialNumber, match, serialRegex)) {
        return match[1]; // La référence produit correspond au premier groupe capturé
    } else {
        throw std::invalid_argument("Invalid serial number format.");
    }
}

std::string ProductDatabase::createSerialNumber(std::string reference) {
    return reference + std::to_string(m_products[reference].size() + 1);
}

bool ProductDatabase::init(const std::string& filename, ReferenceManager& refManager) {
    m_refManager = &refManager;
    m_products.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur: impossible d'ouvrir le fichier JSON " << filename << std::endl;
        return false;
    }

    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        std::cerr << "Erreur de parsing JSON: " << e.what() << std::endl;
        return false;
    }

    if (!j.is_object()) {
        std::cerr << "Erreur: le fichier JSON doit contenir un tableau de produits." << std::endl;
        return false;
    }

    for (auto& [key, value] : j.items()) {
        if (!value.is_number_integer()) {
            std::cerr << "Erreur: la clé \"" << key 
                      << "\" n'a pas une valeur de type entier.\n";
            return false;
        }

        auto referenceResult = refManager.findProductByReference(key);
        if (referenceResult != nullptr) {
            int productNumber = value.get<int>();
            if (productNumber < 0) {
                std::cerr << "Product number can\'t be negative.";
                continue;
            }
            createProduct(referenceResult->getReference(), productNumber);
        }
    }

    return true;
}


ProductDatabase::ProductDatabase(const std::string& filename, ReferenceManager& refManager) {
    init(filename, refManager);
}


// ------------------------------------------------------------------
// Créer "number" produits pour une référence donnée
// ------------------------------------------------------------------

void ProductDatabase::createProduct(const std::string& reference, int number)
{
    if (!m_refManager) {
        std::cerr << "Error: ReferenceManager not initialized.\n";
        return;
    }

    const ProductReference* ref = m_refManager->findProductByReference(reference);
    if (!ref) {
        std::cerr << "Error: No product reference found for '" << reference << "'\n";
        return;
    }

    for (int i = 0; i < number; ++i) {
        std::string serialNumber = createSerialNumber(reference);

        auto product = std::make_unique<Product>(serialNumber, ref->getReference(), Point2D(0.0, 0.0));
        addProduct(std::move(product));
    }
}

void ProductDatabase::moveProduct(const std::string& serialNumber, ProductDatabase& destination) {
    std::string productReference = extractProductReference(serialNumber);

    auto& sourceVec = m_products[productReference];
    if (sourceVec.empty()) {
        std::cerr << "Error: No product found with reference '" << productReference << "'\n";
        return;
    }

    auto& destinationVec = destination.m_products[productReference];
    destinationVec.push_back(std::move(sourceVec.back()));
    sourceVec.pop_back();
}

// ------------------------------------------------------------------
// Ajouter un produit
// ------------------------------------------------------------------
void ProductDatabase::addProduct(std::unique_ptr<Product> product)
{
    const std::string& referenceKey = product->getProductReference();
    auto& productsVec = m_products[referenceKey];

    auto it = std::find_if(productsVec.begin(), productsVec.end(),
                           [&product](const std::unique_ptr<Product>& p) {
                               return p->getSerialNumber() == product->getSerialNumber();
                           });

    if (it != productsVec.end()) {
        std::cerr << "Error: A product with the serial number '"
                  << product->getSerialNumber() << "' already exists.\n";
        return;
    }

    productsVec.push_back(std::move(product));
}

// ------------------------------------------------------------------
// Supprimer un produit par son numéro de série
// ------------------------------------------------------------------
void ProductDatabase::removeProduct(const std::string& serialNumber)
{
    for (auto& [referenceKey, productsVec] : m_products) {
        auto it = std::remove_if(productsVec.begin(), productsVec.end(),
                                 [&serialNumber](const std::unique_ptr<Product>& product) {
                                     return product->getSerialNumber() == serialNumber;
                                 });
        if (it != productsVec.end()) {
            productsVec.erase(it, productsVec.end());
            std::cout << "Product with serial number '" << serialNumber << "' removed.\n";
            return;
        }
    }

    std::cerr << "Error: No product found with the serial number '"
              << serialNumber << "'.\n";
}

// ------------------------------------------------------------------
// Trouver un produit par numéro de série
// ------------------------------------------------------------------
Product* ProductDatabase::findProduct(const std::string& serialNumber) {
    for (auto& [referenceKey, productsVec] : m_products) {
        auto it = std::find_if(productsVec.begin(), productsVec.end(),
                               [&serialNumber](const std::unique_ptr<Product>& product) {
                                   return product->getSerialNumber() == serialNumber;
                               });
        if (it != productsVec.end()) {
            return it->get();
        }
    }
    return nullptr;
}

// ------------------------------------------------------------------
// Lister tous les produits
// ------------------------------------------------------------------
void ProductDatabase::listProducts() const
{
    if (m_products.empty()) {
        std::cout << "The product database is empty.\n";
        return;
    }

    for (const auto& [referenceKey, productsVec] : m_products) {
        for (const auto& product : productsVec) {
            std::cout << "Serial Number: " << product->getSerialNumber() << "\n"
                      << "Product Reference: " << product->getProductReference() << "\n"
                      << "Coordinates: (" << product->getCoordinates() << ")\n\n";
        }
    }
}

// ------------------------------------------------------------------
// Obtenir le nombre total de produits
// ------------------------------------------------------------------
size_t ProductDatabase::getProductCount() const
{
    size_t count = 0;
    for (const auto& [ref, productsVec] : m_products) {
        count += productsVec.size();
    }
    return count;
}

// ------------------------------------------------------------------
// Lister les produits par catégorie
// ------------------------------------------------------------------
void ProductDatabase::listProductsByCategory() const
{
    if (m_products.empty()) {
        std::cout << "The product database is empty.\n";
        return;
    }

    std::unordered_map<std::string, int> categoryCount;
    // On parcourt tous les produits
    for (const auto& [ref, productsVec] : m_products) {
        auto refProduct = m_refManager->findProductByReference(ref);
        if (refProduct == nullptr) continue;

        for (const auto& product : productsVec) {
            categoryCount[refProduct->getCategory()]++;
        }
    }

    std::cout << "Products by category:\n";
    for (const auto& [category, count] : categoryCount) {
        std::cout << "- " << category << ": " << count << " products\n";
    }
}
