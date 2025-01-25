#ifndef TASK_HPP_
#define TASK_HPP_

#include <iostream>
#include <functional>
#include <chrono>
#include <thread>
#include "action.hpp"


struct ActionDescription {
    std::string name; // Action name
    long long processingTime; // Execution time
    int order; // Order of the action
};


class Task {
private:
    std::string m_name;
    std::vector<std::shared_ptr<Actionnable>> m_actions;
    bool m_hasExecuted = false;

public:
    Task() = default;

    explicit Task(const std::string& name)
        : m_name(name) {}

    void addAction(std::shared_ptr<Actionnable> action) {
        m_actions.push_back(std::move(action));
    }

    template <typename... Actions>
    Task(const std::string& name, std::shared_ptr<Actions>... actions)
        : m_name(name) {
        (addAction(actions), ...);
    }

    void execute() {
        if (!m_hasExecuted) {
            for (const auto& action : m_actions) {
                action->execute();
            }
        }
        m_hasExecuted = true;
    }

    bool hasExecuted() {
        return m_hasExecuted;
    }

    std::vector<std::shared_ptr<Actionnable>> getActions() {
        return m_actions;
    }

    long long getDuration() const {
        long long duration = 0;
        for (const auto& action : m_actions) {
            duration += action->getProcessingTime();
        }
        return duration;
    }

    std::string getName() const { return m_name; }

    std::vector<ActionDescription> getActionDescriptions() const {
        std::vector<ActionDescription> descriptions;
        int order = 0;

        for (const auto& action : m_actions) {
            descriptions.push_back({
                action->getName(),           // Assume Actionnable has a getName() method
                action->getProcessingTime(), // Assume Actionnable has a getProcessingTime() method
                ++order                      // Increment order for each action
            });
        }

        return descriptions;
    }
};


#endif
