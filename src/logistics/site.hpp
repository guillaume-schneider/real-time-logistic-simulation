#ifndef SITE_HPP_
#define SITE_HPP_

#include <vector>
#include "location.hpp"
#include "coordinates.hpp"
#include "../stock/product/product_database.hpp"

class SiteArea {
public:
    Point2D coordinates;
};

class DeliveryPlatform : public SiteArea {

};

class StorageSpace : public SiteArea {
private:
    ProductDatabase m_db;
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
};

#endif
