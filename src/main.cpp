#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/action.hpp"
#include <vector>
#include <memory>
#include <thread>
#include "parameters.hpp"


long long convertTimeInSeconds(const Time& time) {
    long long totalSeconds = static_cast<long long>(time.days) * 86400
                        + static_cast<long long>(time.hours) * 3600
                        + static_cast<long long>(time.minutes) * 60
                        + static_cast<long long>(time.seconds);
    return totalSeconds;
}


void verifyTimescale(Config& config) {
    if (config.timescale <= 0.0f) {
        std::cerr << "Timescale cannot be negative or equal to 0." << std::endl;
        config.timescale = 0.0001f;
    } 
}


int main() {
    std::cout << "\33[2J";

    std::mutex outputMutex;
    ConfigParser parser;
    Config config = parser.parseConfig("config.json");
    auto realDuration = convertTimeInSeconds(config.time);
    verifyTimescale(config);

    std::vector<std::shared_ptr<Actionner>> actionners;

    std::shared_ptr<Actionner> actionner = std::make_shared<Actionner>(1, "Actionner 1", outputMutex, config);
    std::shared_ptr<Actionner> actionner2 = std::make_shared<Actionner>(2, "Actionner 2", outputMutex, config);

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

    for (size_t i = 0; i < tasks.size(); ++i) {
        // if (!actionner->busy()) {
            actionner->submitTask(tasks[i]);
        // }
    }

    for (size_t i = 0; i < tasks.size(); ++i) {
        // if (!actionner->busy()) {
            actionner2->submitTask(tasks[i]);
        // }
    }

    auto simDuration = realDuration / config.timescale;
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= simDuration) break;
    }

    // Attendre que toutes les tâches soient terminées
    // std::cout << "Press Enter to exit...\n";
    // std::cin.get();

    return 0;
}


// int main()
// {
//     int jours, heures, minutes, secondes;
    
//     std::cout << "Entrez le nombre de jours : ";
//     std::cin >> jours;
//     std::cout << "Entrez le nombre d'heures : ";
//     std::cin >> heures;
//     std::cout << "Entrez le nombre de minutes : ";
//     std::cin >> minutes;
//     std::cout << "Entrez le nombre de secondes : ";
//     std::cin >> secondes;

//     Config config;
    
//     // Conversion en secondes totales
//     long long totalSeconds = static_cast<long long>(jours) * 86400
//                            + static_cast<long long>(heures) * 3600
//                            + static_cast<long long>(minutes) * 60
//                            + static_cast<long long>(secondes);
//     float timescale = config.timescale;
//     if (timescale <= 0.0f) timescale = 0.001f;
//     long long durationSim = totalSeconds / timescale;

//     // Point de départ
//     auto start = std::chrono::steady_clock::now();
    
//     while (true)
//     {
//         // Vérifie le temps écoulé
//         auto now = std::chrono::steady_clock::now();
//         auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

//         if (elapsed >= durationSim) break;
        
//         // --- Placez ici le traitement à effectuer dans votre boucle ---
        
//         // Pour éviter de saturer le CPU si la boucle fait peu de choses
//     }
    
//     return 0;
// }


// #include <iostream>
// #include "logistics/product_database.hpp"

// int main() {
//     ProductDatabase db;

//     ProductReference ref1("Laptop", "Electronics", "Computers", "2025", "REF12345");
//     ProductReference ref2("Smartphone", "Electronics", "Mobile Phones", "2025", "REF67890");
//     ProductReference ref3("Headphones", "Accessories", "Audio", "2025", "REF11223");

//     Product product1("SN001", ref1, Point2D{10.0, 20.0});
//     Product product2("SN002", ref2, Point2D{30.0, 40.0});
//     Product product3("SN003", ref3, Point2D{50.0, 60.0});
//     Product product4("SN004", ref2, Point2D{70.0, 80.0}); // Another smartphone

//     db.addProduct(product1);
//     db.addProduct(product2);
//     db.addProduct(product3);
//     db.addProduct(product4);

//     std::cout << "Listing products by category:\n";
//     db.listProductsByCategory();

//     std::cout << "\nListing products by sub-category:\n";
//     db.listProductsBySubCategory();

//     db.listProducts();

//     return 0;
// }


// #include <iostream>
// #include <mutex>
// #include "logistics/conveyor.hpp"
// #include "logistics/task.hpp"
// #include "parameters.hpp"
// #include "logistics/location.hpp"

// int main() {
//     std::mutex outputMutex;
//     Config config;
//     config.timescale = 1.0f; // Normal speed

//     // Create a Conveyor (child of Actionner)
//     Conveyor myConveyor(0, "Conveyor #1", outputMutex, config, Location());

//     std::vector<std::shared_ptr<Actionnable>> actions = {
//         std::make_shared<Move>(3000),
//         std::make_shared<Move>(5000),
//         std::make_shared<Move>(2000)
//     };

//     // Create multiple tasks
//     Task t1("Box A"); // 3s simulated
//     Task t2("Box B"); // 4s simulated
//     Task t3("Box C"); // 2s simulated

//     t1.addAction(actions[0]);
//     t2.addAction(actions[1]);
//     t3.addAction(actions[2]);

//     // Submit tasks
//     myConveyor.submitTask(t1);
//     myConveyor.submitTask(t2);
//     myConveyor.submitTask(t3);

//     std::cout << "\nAll tasks have completed.\n\n";
//     return 0;
// }
