#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/actions.hpp"
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
#include "logistics/scheduler.hpp"
#include "logistics/site.hpp"
#include <atomic>
#include "shell.hpp"


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


int main(int argc, char* argv[]) {
    clear();
    Scheduler& scheduler = Scheduler::getInstance();
    Shell shell;
    scheduler.setOutputMutex(shell.getOutputMutex());
    scheduler.setShowLoadingAtomic(shell.getShowLoadingBars());

    if (!Initializer::getInstance().injectArguments(argc, argv)) return 1;
    ReferenceManager& refManager = ReferenceManager::getInstance();
    Site& site = Site::getInstance();
    OrderDatabase& orderDb = OrderDatabase::getInstance();
    Parameters parameters;
    Initializer::getInstance().loadData(refManager, orderDb,
                                        parameters, scheduler, site);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(5, 100);
    AddressDatabase& addressDb = AddressDatabase::getInstance();
    for (auto order : orderDb.getOrders()) {
        addressDb.addAddress(order->deliveryAddress, dis(gen), dis(gen));
        scheduler.affectOrder(order);
    }

    auto realDuration = convertTimeInSeconds(parameters.time);
    verifyTimescale(parameters);

    auto simDuration = realDuration / parameters.timescale;
    auto start = std::chrono::steady_clock::now();
    scheduler.runScheduler();
    shell.run();
    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= simDuration) break;
        if (!scheduler.hasRemainingTask() && scheduler.areWorkersIdle()) break;
        if(shell.getStopShell().load()) break;
    }
    scheduler.stopScheduler();

    return 0;
}
