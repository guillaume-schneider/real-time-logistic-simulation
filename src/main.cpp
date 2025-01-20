#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/action.hpp"
#include <vector>
#include <memory>
#include <thread>
#include "parameters.hpp"
#include "product/product_database.hpp"
#include "product/reference_manager.hpp"


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
    ReferenceManager& refManager = ReferenceManager::getInstance();
    refManager.loadFromJson("references.json");

    ProductDatabase& dbProduct = ProductDatabase::getInstance();
    if (!dbProduct.init("products.json", refManager)) {
        std::cerr << "Failed to initialize the database.\n";
        return 1;
    }

    // 3) Créer des produits par référence (en lot)
    //    createProduct(reference, number)
    dbProduct.createProduct("ELPH240002", 3);   // Crée 3 produits
    dbProduct.createProduct("ELPH240001", 2);   // Crée 2 produits
    dbProduct.createProduct("XYZ123", 1);       // Crée 1 produit

    // 5) Lister tous les produits
    std::cout << "\n--- List of all products ---\n";
    dbProduct.listProducts();
    std::cout << "Total products: " << dbProduct.getProductCount() << "\n\n";

    // 6) Lister les produits par catégorie
    std::cout << "\n--- Products by category ---\n";
    dbProduct.listProductsByCategory();

    // 7) Rechercher un produit par serial number (exemple)
    //    Selon la logique de createProduct, les serials 
    //    peuvent ressembler à "ELPH240002_1", "ELPH240002_2", etc.
    Product* found = dbProduct.findProduct("ELPH2400021");
    if (found) {
        std::cout << "\nProduct ELPH2400021 found: "
                  << found->getSerialNumber() << " / "
                  << found->getProductReference() << "\n";
    } else {
        std::cerr << "\nProduct ELPH2400021 not found.\n";
    }

    // 8) Supprimer un produit par son numéro de série
    dbProduct.removeProduct("ELPH2400021");

    // 9) Vérifier que la suppression a bien eu lieu
    found = dbProduct.findProduct("ELPH2400021");
    if (!found) {
        std::cout << "Removal confirmed: ELPH240002_1 not found.\n";
    } else {
        std::cerr << "Error: ELPH240002_1 still found after removal.\n";
    }

    // 10) Lister les produits restants
    std::cout << "\n--- List of products after removal ---\n";
    dbProduct.listProducts();

    // std::cout << "\33[2J";

    // std::mutex outputMutex;
    // ConfigParser parser;
    // Config config = parser.parseConfig("config.json");
    // auto realDuration = convertTimeInSeconds(config.time);
    // verifyTimescale(config);

    // std::shared_ptr<Actionner> actionner = std::make_shared<Actionner>(1, "Actionner 1", outputMutex, config);
    // std::shared_ptr<Actionner> actionner2 = std::make_shared<Actionner>(2, "Actionner 2", outputMutex, config);

    // std::vector<std::shared_ptr<Actionnable>> actions = {
    //     std::make_shared<Move>(3000),
    //     std::make_shared<Move>(5000),
    //     std::make_shared<Move>(2000)
    // };

    // std::vector<std::shared_ptr<Task>> tasks = {
    //     std::make_shared<Task>("Move 1", actions[0]),
    //     std::make_shared<Task>("Move 2", actions[1]),
    //     std::make_shared<Task>("Move 3", actions[2])
    // };

    // for (auto& task : tasks) {
    //     actionner->submitTask(task);
    //     actionner2->submitTask(task);
    // }

    // auto simDuration = realDuration / config.timescale;
    // auto start = std::chrono::steady_clock::now();
    // while (true)
    // {
    //     auto now = std::chrono::steady_clock::now();
    //     auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    //     if (elapsed >= simDuration) break;
    // }

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
