#ifndef ACTION_HPP_
#define ACTION_HPP_

#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <string>

class Actionnable {
protected:
    int processingTime;
public:
    explicit Actionnable(int time) : processingTime(time) {}
    virtual ~Actionnable() = default;

    virtual void execute() = 0;
    int getProcessingTime() { return processingTime; }

    void displayProcessingTime() const {
        std::cout << "Processing time: " << processingTime << " ms" << std::endl;
    }
};

class Move : public Actionnable {
public:
    explicit Move(int time) : Actionnable(time) {}

    void execute() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(processingTime));
    }
};

class ActionB : public Actionnable {
public:
    explicit ActionB(int time) : Actionnable(time) {}

    void execute() override {
        for (int i = 0; i < processingTime / 100; ++i) {
            std::cout << "."; // Simule un travail en cours
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};


class Task {
private:
    int id;
    std::vector<std::shared_ptr<Actionnable>> actions;

    void clearLine() const {
        std::cout << "\33[2K\r";
    }

    void loadingBar(int totalDuration, std::atomic<bool>& isRunning, std::atomic<bool>& actionsComplete, std::mutex& outputMutex) const {
        const int barWidth = 50;
        float progress = 0.0f;

        auto startTime = std::chrono::steady_clock::now();
        while (isRunning.load()) {
            // Calculate elapsed time
            auto now = std::chrono::steady_clock::now();
            progress = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count()) / totalDuration;
            progress = std::min(progress, 1.0f); // Cap progress to 100%

            // Build the loading bar
            std::string bar = "[";
            int pos = static_cast<int>(barWidth * progress);
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) bar += "=";
                else if (i == pos) bar += ">";
                else bar += " ";
            }
            bar += "] " + std::to_string(int(progress * 100)) + " %";

            // Display the bar (use mutex for synchronized output)
            {
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cout << "\33[" << (id + 1) << ";1H" << bar << std::flush; // Position bar on the correct line
            }

            // Pause at 100% if actions are not complete
            if (progress >= 1.0f && !actionsComplete.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Exit when actions are complete and progress is 100%
            if (progress >= 1.0f && actionsComplete.load()) {
                break;
            }
        }

        // Mark as complete
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "\33[" << (id + 1) << ";1H[";
            for (int i = 0; i < barWidth; ++i) std::cout << "=";
            std::cout << "] 100 % - Task " << id << " complete!\n" << std::flush;
        }
    }


public:
    Task(int id) : id(id) {}

    void addAction(std::shared_ptr<Actionnable> action) {
        actions.push_back(action);
    }

    int getTotalDuration() const {
        int totalDuration = 0;
        for (const auto& action : actions) {
            totalDuration += action->getProcessingTime();
        }
        return totalDuration;
    }

    void execute(std::mutex& outputMutex) const {
        int totalDuration = getTotalDuration();

        std::atomic<bool> isRunning(true);
        std::atomic<bool> actionsComplete(false);
        std::thread loadingThread;

        // Start the loading bar in a separate thread
        loadingThread = std::thread(&Task::loadingBar, this, totalDuration, std::ref(isRunning), std::ref(actionsComplete), std::ref(outputMutex));

        // Execute actions
        for (const auto& action : actions) {
            action->execute();
        }

        // Mark actions as complete
        actionsComplete = true;

        // Stop the loading bar
        isRunning = false;
        if (loadingThread.joinable()) {
            loadingThread.join();
        }
    }
};

#endif
