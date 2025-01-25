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
        auto movingTime = calculateTravelTimeInSeconds(worker->getCoordinates(), storageCoordinates, worker->getSpeed());
        result->addAction(std::make_shared<Move>("Move", movingTime, worker, storageCoordinates));
        return result;
    }
};

#endif
