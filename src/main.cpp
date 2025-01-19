#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/action.hpp"
#include <vector>
#include <memory>
#include <thread>
#include "parameters.hpp"


int main() {
    std::cout << "\33[2J";

    std::mutex outputMutex;
    ConfigParser parser;
    Config config = parser.parseConfig("config.json");

    std::vector<std::shared_ptr<Actionner>> actionners;
    // for (int i = 0; i < 3; ++i) {
    //     actionners.push_back(std::make_shared<Actionner>(i, "Actionner" + std::to_string(i), outputMutex, config));
    // }

    std::shared_ptr<Actionner> actionner = std::make_shared<Actionner>(1, "Actionner 1", outputMutex, config);
    std::shared_ptr<Actionner> actionner2 = std::make_shared<Actionner>(2, "Actionner 2", outputMutex, config);

    std::vector<std::shared_ptr<Actionnable>> actions = {
        std::make_shared<Move>(3000),
        std::make_shared<Move>(5000),
        std::make_shared<Move>(2000)
    };

    std::vector<Task> tasks = {
        Task("Move 1", actions[0]),
        Task("Move 2", actions[1]),
        Task("Move 3", actions[2])
    };

    // Assigner les tâches aux Actionners disponibles
    auto start = std::chrono::steady_clock::now();
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

    while(true);

    auto end = std::chrono::steady_clock::now();    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    int hours = duration.count() / 3600;
    int minutes = (duration.count() % 3600) / 60;
    int seconds = duration.count() % 60;

    std::cout << "Elapsed time: "
              << hours << "h "
              << minutes << "m "
              << seconds << "s" << std::endl;

    // Attendre que toutes les tâches soient terminées
    // std::cout << "Press Enter to exit...\n";
    // std::cin.get();

    return 0;
}
