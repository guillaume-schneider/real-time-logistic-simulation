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

std::atomic<bool> stopShell(false);

// Fonction pour gérer l'entrée de commandes utilisateur
void commandShell(std::mutex& output) {
    std::string command;

    while (!stopShell.load()) {
        // Afficher la ligne de commande en haut
        {
            std::lock_guard<std::mutex> lock(output);
            std::cout << "\33[H\33[2K"; // Effacer la première ligne
            std::cout << "Command > ";  // Afficher le prompt
            std::cout.flush();
        }

        // Lire la commande utilisateur
        std::getline(std::cin, command);

        // Vérifier la commande
        if (command == "exit") {
            stopShell.store(true);
        } else if (command == "status") {
            std::lock_guard<std::mutex> lock(output);
            std::cout << "\33[2K\33[H"; // Effacer la première ligne
            std::cout << "Status: All systems running.\n";
            std::cout.flush();
        } else {
            std::lock_guard<std::mutex> lock(output);
            std::cout << "\33[2K\33[H"; // Effacer la première ligne
            std::cout << "Unknown command: " << command << "\n";
            std::cout.flush();
        }
    }
}


int main(int argc, char* argv[]) {
    clear();
    std::shared_ptr<std::mutex> outputMutex = std::make_shared<std::mutex>();
    Scheduler& scheduler = Scheduler::getInstance();
    scheduler.setOutputMutex(outputMutex);

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
    std::thread shellThread(commandShell, outputMutex);
    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= simDuration) break;
        if (!scheduler.hasRemainingTask() && scheduler.areWorkersIdle()) break;
    }
    scheduler.stopScheduler();

    shellThread.join();

    return 0;
}
