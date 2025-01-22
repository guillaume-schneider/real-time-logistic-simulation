#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/action.hpp"
#include <vector>
#include <memory>
#include <thread>
#include "parameters.hpp"
#include "product/product_database.hpp"
#include "product/reference_manager.hpp"
#include "product/order.hpp"
#include "product/order_database.hpp"
#include "date/date.h"
#include "initializer.hpp"


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
    if (!Initializer::getInstance().injectArguments(argc, argv)) return 1;
    ReferenceManager& refManager = ReferenceManager::getInstance();
    ProductDatabase productDb;
    OrderDatabase orderDb;
    Parameters parameters;
    Initializer::getInstance().loadData(refManager, productDb, orderDb, parameters);

    std::cout << "\n--- List of all products ---\n";
    productDb.listProducts();
    std::cout << "Total products: " << productDb.getProductCount() << "\n\n";

    // 6) Lister les produits par catégorie
    std::cout << "\n--- Products by category ---\n";
    productDb.listProductsByCategory();

    // 7) Rechercher un produit par serial number (exemple)
    //    Selon la logique de createProduct, les serials 
    //    peuvent ressembler à "ELPH240002_1", "ELPH240002_2", etc.
    Product* found = productDb.findProduct("ELPH2400021");
    if (found) {
        std::cout << "\nProduct ELPH2400021 found: "
                  << found->getSerialNumber() << " / "
                  << found->getProductReference() << "\n";
    } else {
        std::cerr << "\nProduct ELPH2400021 not found.\n";
    }

    // 8) Supprimer un produit par son numéro de série
    productDb.removeProduct("ELPH2400021");

    // 9) Vérifier que la suppression a bien eu lieu
    found = productDb.findProduct("ELPH2400021");
    if (!found) {
        std::cout << "Removal confirmed: ELPH240002_1 not found.\n";
    } else {
        std::cerr << "Error: ELPH240002_1 still found after removal.\n";
    }

    // 10) Lister les produits restants
    std::cout << "\n--- List of products after removal ---\n";
    productDb.listProducts();

    std::cout << "\33[2J";

    std::mutex outputMutex;
    auto realDuration = convertTimeInSeconds(parameters.time);
    verifyTimescale(parameters);

    std::shared_ptr<Actionner> actionner = std::make_shared<Actionner>(1, "Actionner 1", outputMutex, parameters);
    std::shared_ptr<Actionner> actionner2 = std::make_shared<Actionner>(2, "Actionner 2", outputMutex, parameters);

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

    for (auto& task : tasks) {
        actionner->submitTask(task);
        actionner2->submitTask(task);
    }

    auto simDuration = realDuration / parameters.timescale;
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= simDuration) break;
    }

    // std::cout << "Press Enter to exit...\n";
    // std::cin.get();

    OrderDatabase db;

    try {
        db.loadFromFile("./orders.json");
        std::cout << "Commandes chargées depuis orders.json :\n";
        auto orders = db.getOrders();
        for (const auto& order : orders) {
            std::cout << order.toString() << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << "\n";
        return 1;
    }

    return 0;
}
