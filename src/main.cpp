#include <iostream>
#include "logistics/action.hpp"
#include <chrono>

int main() {
    // Clear the console
    std::cout << "\33[2J";

    // Create tasks
    std::vector<std::shared_ptr<Task>> tasks;
    tasks.push_back(std::make_shared<Task>(0)); // Task 0
    tasks.push_back(std::make_shared<Task>(1)); // Task 1
    tasks.push_back(std::make_shared<Task>(2)); // Task 2

    // Add actions to each task
    tasks[0]->addAction(std::make_shared<Move>(3000)); // 3 seconds
    tasks[1]->addAction(std::make_shared<Move>(5000)); // 5 seconds
    tasks[2]->addAction(std::make_shared<Move>(7000)); // 7 seconds

    // Mutex for synchronized output
    std::mutex outputMutex;

    // Run tasks in parallel
    std::vector<std::thread> threads;
    for (const auto& task : tasks) {
        threads.emplace_back(&Task::execute, task, std::ref(outputMutex));
    }

    // Wait for all tasks to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    std::cout << "\nAll tasks completed!\n";
    return 0;
}
