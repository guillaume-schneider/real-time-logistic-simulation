#ifndef SITE_HPP_
#define SITE_HPP_

#include <vector>
#include "location.hpp"
#include "coordinates.hpp"
#include "../stock/product/product_database.hpp"
#include "../stock/reference/reference_manager.hpp"
#include <memory>

class SiteArea {
public:
    Point2D coordinates;
};

class DeliveryPlatform : public SiteArea {
private:

public:

};

class StorageSpace : public SiteArea {
private:
    ProductDatabase m_db;
public:
    StorageSpace() = default;
    bool init(const std::string& filename) {
        return m_db.init(filename, ReferenceManager::getInstance());
    }

    std::unique_ptr<Product> getProduct(const std::string& serialNumber) {
        return m_db.getProduct(serialNumber);
    }

    std::unique_ptr<Product> getProductByCategoryAndSubcat(const std::string& category, const std::string& sub) {
        return std::move(m_db.getProductByCategoryAndSubcategory(category, sub));
    }
};

class Site : public Location {
private:
    Site() = default;
public:
    DeliveryPlatform delivery;
    StorageSpace storage;

    static Site& getInstance() {
        static Site instance;
        return instance;
    }
    Site(const Site&) = delete;
    Site& operator=(const Site&) = delete;

    bool init(const std::string& productFile) {
        return storage.init(productFile);
    }
};

#endif
