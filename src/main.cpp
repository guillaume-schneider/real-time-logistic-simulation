#include "logistics/actionner.hpp"
#include "logistics/task.hpp"
#include "logistics/action.hpp"
#include <vector>
#include <memory>

int main() {
    std::cout << "\33[2J";

    std::mutex outputMutex;

    std::vector<std::shared_ptr<Actionner>> actionners;
    for (int i = 0; i < 3; ++i) {
        actionners.push_back(std::make_shared<Actionner>(i, "Actionner" + std::to_string(i), outputMutex));
    }

    std::vector<std::shared_ptr<Actionnable>> actions = {
        std::make_shared<Move>(3000),
        std::make_shared<Move>(5000),
        std::make_shared<Move>(2000)
    };

    std::vector<Task> tasks = {
        Task("Move 1", actions[0]),
        Task("Move 2", actions[1]),
        Task("Move 2", actions[2])
    };

    // Assigner les tâches aux Actionners disponibles
    for (size_t i = 0; i < tasks.size(); ++i) {
        auto& actionner = actionners[i];
        if (!actionner->busy()) {
            actionner->submitTask(tasks[i]);
        }
    }

    // Attendre que toutes les tâches soient terminées
    std::cout << "Press Enter to exit...\n";
    std::cin.get();

    return 0;
}
