#ifndef TASK_FACTORY_HPP_
#define TASK_FACTORY_HPP_

#include "task.hpp"
#include "../stock/order/order.hpp"
#include "worker.hpp"
#include "coordinates.hpp"
#include "address_database.hpp"
#include "site.hpp"
#include "actions.hpp"
#include <memory>
#include <random>

class TaskFactory {
private:
    static long long getRandomIntInRange(long long min, long long max) {
        if (min > max) {
            throw std::invalid_argument("Minimum value cannot be greater than maximum value.");
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<long long> dist(min, max);

        return dist(gen);
    }
public:
    static std::shared_ptr<Task> createPrepareOrderTask(std::shared_ptr<Order> order, std::shared_ptr<Worker> worker) {
        auto result = std::make_shared<Task>("Prepare Order");
        auto storageCoordinates = Site::getInstance().storage.coordinates;
        auto movingTimeStorage = calculateTravelTimeInMs(
            worker->getCoordinates(),
            storageCoordinates,
            worker->getSpeed()
        );
        result->addAction(std::make_shared<Move>(
            "Move to Storage",
            movingTimeStorage, 
            worker, 
            storageCoordinates
        ));

        auto& site = Site::getInstance();

        auto takeProductTime = getRandomIntInRange(4000, 8000);
        auto productReference = order->productReference;
        result->addAction(std::make_shared<TakeProduct>(
            "Take Product " + productReference,
            takeProductTime,
            worker,
            &site.storage,
            order
        ));

        auto deliveryCoordinates = Site::getInstance().delivery.coordinates;
        auto movingTimeDelivery = calculateTravelTimeInMs(storageCoordinates, deliveryCoordinates, worker->getSpeed());
        result->addAction(std::make_shared<Move>(
            "Move to Delivery", 
            movingTimeDelivery, 
            worker, 
            deliveryCoordinates        ));

        auto releaseProductTime = getRandomIntInRange(4000, 8000);
        result->addAction(std::make_shared<ReleaseProduct>(
            "Release Product " + productReference, 
            releaseProductTime, 
            worker, 
            &site.delivery
        ));

        auto siteCoordinates = Site::getInstance().coordinates;
        auto movingTimeSite = calculateTravelTimeInMs(deliveryCoordinates, siteCoordinates, worker->getSpeed());
        result->addAction(std::make_shared<Move>(
            "Move to Site",
            movingTimeDelivery, 
            worker, 
            siteCoordinates
        ));
        return result;
    }
};

#endif
