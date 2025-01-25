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

class TaskFactory {
public:
    static std::shared_ptr<Task> createPrepareOrderTask(const Order& order, std::shared_ptr<Worker> worker) {
        auto result = std::make_shared<Task>("Prepare Order");
        auto storageCoordinates = Site::getInstance().storage.coordinates;
        auto movingTimeStorage = calculateTravelTimeInMs(worker->getCoordinates(), storageCoordinates, worker->getSpeed());
        result->addAction(std::make_shared<Move>("Move to Storage", movingTimeStorage, worker, storageCoordinates));

        auto deliveryCoordinates = Site::getInstance().delivery.coordinates;
        auto movingTimeDelivery = calculateTravelTimeInMs(worker->getCoordinates(), deliveryCoordinates, worker->getSpeed());
        result->addAction(std::make_shared<Move>("Move to Delivery", movingTimeDelivery, worker, deliveryCoordinates));
        return result;
    }
};

#endif
