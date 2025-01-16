#ifndef TASK_HPP_
#define TASK_HPP_

#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include "action.hpp"

class Task {
private:
    std::string m_name;
    std::vector<std::shared_ptr<Actionnable>> m_actions;

public:
    // Constructor with task name
    explicit Task(const std::string& name)
        : m_name(name) {}

    // Add an action via shared_ptr
    void addAction(std::shared_ptr<Actionnable> action) {
        m_actions.push_back(std::move(action));
    }

    // Variadic constructor to add multiple actions
    template <typename... Actions>
    Task(const std::string& name, std::shared_ptr<Actions>... actions)
        : m_name(name) {
        (addAction(actions), ...); // Use fold expression for multiple shared_ptr
    }

    // Execute the task
    void execute() const {
        int duration = getDuration();
        std::cout << "Executing task '" << m_name << "' for " << duration << " ms.\n";
        for (const auto& action : m_actions) {
            action->execute(); // Execute each action
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }

    // Get a callable function for the task
    std::function<void()> getTaskFunction() const {
        return [this]() { this->execute(); };
    }

    // Calculate the total duration of all actions
    int getDuration() const {
        int duration = 0;
        for (const auto& action : m_actions) {
            duration += action->getProcessingTime();
        }
        return duration;
    }

    std::string getName() { return m_name; }
};


#endif // TASK_HPP_
