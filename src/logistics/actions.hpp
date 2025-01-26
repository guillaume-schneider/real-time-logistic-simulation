#ifndef ACTIONS_HPP_
#define ACTIONS_HPP_

#include "action.hpp"
#include "worker.hpp"
#include "coordinates.hpp"
#include "site.hpp"
#include <memory>

class Move : public Actionnable {
private:
    std::shared_ptr<Worker> m_actionner;
    Point2D m_destination;
public:
    explicit Move(const std::string& name, long long time,
        std::shared_ptr<Worker> movable, const Point2D& destination)
         : Actionnable(name, time), m_actionner(movable), m_destination(destination) {}

    void execute() override {
        m_actionner->setCoordinates(m_destination);
    }
};

class TakeProduct : public Actionnable {
private:
    std::shared_ptr<Worker> m_actionner;
    StorageSpace* m_storage;
    std::shared_ptr<Order> m_order;
public:
    explicit TakeProduct(const std::string& name, long long time,
        std::shared_ptr<Worker> worker, StorageSpace* storage, std::shared_ptr<Order> order)
        : Actionnable(name, time), m_actionner(worker), m_storage(storage), m_order(order) {}
    TakeProduct(const TakeProduct&) = delete;
    TakeProduct& operator=(const TakeProduct&) = delete;

    void execute() override {
        m_actionner->setOrder(*m_order);
        if (!m_actionner->getOrder()) {
            std::cerr << "GetProduct::execute Error: Worker doesn\'t have order.\n";
            return;
        }
        auto reference = ProductReference::fromString(m_actionner->getOrder()->productReference);
        auto product = std::move(m_storage->getProductByCategoryAndSubcat(reference.getCategory(), reference.getSubCategory()));
        m_actionner->getOrder()->serialNumber = product->getSerialNumber();
        m_actionner->takeProduct(std::move(product));
    }
};

class ReleaseProduct : public Actionnable {
private:
    std::shared_ptr<Worker> m_actionner;
    DeliveryPlatform* m_delivery;
public:
    explicit ReleaseProduct(const std::string& name, long long time,
        std::shared_ptr<Worker> worker, DeliveryPlatform* delivery)
        : Actionnable(name, time), m_actionner(worker), m_delivery(delivery) {}

    void execute() override {
        m_actionner->releaseProduct();
    }
};

#endif
