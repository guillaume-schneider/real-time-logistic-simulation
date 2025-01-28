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


std::optional<std::string> ProductDatabase::extractProductReference(const std::string& serialNumber) {
    std::regex serialRegex("([A-Z]+\\d+\\d{2})(\\d{4})");
    std::smatch match;

    if (std::regex_match(serialNumber, match, serialRegex)) {
        return match[1];
    }

    return std::nullopt;
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

std::unique_ptr<Product> ProductDatabase::getProduct(const std::string& serialNumber) {
    auto productReferenceResult = extractProductReference(serialNumber);
    if (productReferenceResult == std::nullopt) {
        std::cerr << "ProductDatabase::moveProduct Error: No product found with reference '" << productReferenceResult.value() << "'\n";
        return nullptr;
    }

    auto productReference = productReferenceResult.value();

    auto& sourceVec = m_products[productReference];
    if (sourceVec.empty()) {
        std::cerr << "ProductDatabase::moveProduct Error: No product found with reference '" << productReference << "'\n";
        return nullptr;
    }

    auto it = std::find_if(sourceVec.begin(), sourceVec.end(), [&serialNumber](const std::unique_ptr<Product>& product) {
        return product->getSerialNumber() == serialNumber;
    });

    if (it == sourceVec.end()) {
        std::cerr << "ProductDatabase::moveProduct Error: No product found with serial number '" << serialNumber << "'\n";
        return nullptr;
    }

    std::unique_ptr<Product> resultProduct = std::move(*it);
    sourceVec.erase(it);
    return resultProduct;
}


void ProductDatabase::addProduct(std::unique_ptr<Product> product)
{
    const std::string& referenceKey = product->getProductReference();
    auto& productsVec = m_products[referenceKey];

    auto it = std::find_if(productsVec.begin(), productsVec.end(),
                           [&product](const std::unique_ptr<Product>& p) {
                               return p->getSerialNumber() == product->getSerialNumber();
                           });

    if (it != productsVec.end()) {
        std::cerr << "ProductDatabase::addProduct Error:: A product with the serial number '"
                  << product->getSerialNumber() << "' already exists.\n";
        return;
    }

    productsVec.emplace_back(std::move(product));
}


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

    std::cerr << "ProductDatabase::removeProduct Error:: No product found with the serial number '"
              << serialNumber << "'.\n";
}


const Product* ProductDatabase::findProduct(const std::string& serialNumber) {
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


size_t ProductDatabase::getProductCount() const
{
    size_t count = 0;
    for (const auto& [ref, productsVec] : m_products) {
        count += productsVec.size();
    }
    return count;
}


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

std::unique_ptr<Product> ProductDatabase::getProductByCategoryAndSubcategory(const std::string& category, const std::string& subcategory) {
    for (auto& [reference, productVec] : m_products) {
        for (auto it = productVec.begin(); it != productVec.end(); ++it) {
            auto reference = ProductReference::fromString((*it)->getProductReference());
            if (reference.getCategory() == category && reference.getSubCategory() == subcategory) {
                std::unique_ptr<Product> product = std::move(*it);
                productVec.erase(it);
                return std::move(product);
            }
        }
    }

    // No matching product found
    std::cerr << "ProductDatabase::getProductByCategoryAndSubcategory Error: No product found for category: " << category << " and subcategory: " << subcategory << std::endl;
    return nullptr;
}
