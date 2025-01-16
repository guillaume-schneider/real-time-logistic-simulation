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
#include <functional>

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

#endif
