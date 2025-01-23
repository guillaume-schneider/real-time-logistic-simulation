#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/action.hpp"
#include <vector>
#include <memory>
#include <thread>
#include "parameters.hpp"
#include "stock/product/product_database.hpp"
#include "stock/reference/reference_manager.hpp"
#include "stock/order/order.hpp"
#include "stock/order/order_database.hpp"
#include "date/date.h"
#include "initializer.hpp"
#include "logistics/address_database.hpp"
#include <random>
#include <cmath>
#include "logistics/ordonator.hpp"


long long convertTimeInSeconds(const Time& time) {
    long long totalSeconds = static_cast<long long>(time.days) * 86400
                        + static_cast<long long>(time.hours) * 3600
                        + static_cast<long long>(time.minutes) * 60
                        + static_cast<long long>(time.seconds);
    return totalSeconds;
}


void verifyTimescale(Parameters& config) {
    if (config.timescale <= 0.0f) {
        std::cerr << "Timescale cannot be negative or equal to 0." << std::endl;
        config.timescale = 0.0001f;
    } 
}


void clear() {
    std::cout << "\33[2J";
}


int main(int argc, char* argv[]) {
    clear();
    std::mutex outputMutex;
    Ordonator& ordonator = Ordonator::getInstance();
    ordonator.setOutputMutex(&outputMutex);

    if (!Initializer::getInstance().injectArguments(argc, argv)) return 1;
    ReferenceManager& refManager = ReferenceManager::getInstance();
    ProductDatabase productDb;
    OrderDatabase orderDb;
    Parameters parameters;
    Initializer::getInstance().loadData(refManager, productDb, orderDb,
                                        parameters, ordonator);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(5, 100);
    AddressDatabase& addressDb = AddressDatabase::getInstance();
    for (const auto& order : orderDb.getOrders()) {
        addressDb.addAddress(order.deliveryAddress, dis(gen), dis(gen));
    }

    auto realDuration = convertTimeInSeconds(parameters.time);
    verifyTimescale(parameters);

    std::vector<std::shared_ptr<Actionnable>> actions = {
        std::make_shared<Move>(3000),
        std::make_shared<Move>(5000),
        std::make_shared<Move>(2000)
    };

    std::vector<std::shared_ptr<Task>> tasks = {
        std::make_shared<Task>("Move 1", actions[0]),
        std::make_shared<Task>("Move 2", actions[1]),
        std::make_shared<Task>("Move 3", actions[2])
    };

    auto id = ordonator.getIdleWorker();
    for (auto& task : tasks) {
        ordonator.affectTaskToWorker(task, id);
    }

    auto simDuration = realDuration / parameters.timescale;
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= simDuration) break;
    }

    return 0;
}
