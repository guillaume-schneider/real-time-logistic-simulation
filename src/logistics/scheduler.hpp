#ifndef ORDONATOR_HPP_
#define ORDONATOR_HPP_

#include <string>
#include <nlohmann/json.hpp>
#include <vector>
#include "worker.hpp"
#include "tool_type.hpp"
#include "../parameters.hpp"
#include <memory>
#include <fstream>
#include <thread>
#include <atomic>
#include <queue>

constexpr int refreshThreadTimeInMs = 100;

class Scheduler {
private:
    static std::atomic<int> m_workerCounter;
    Point2D m_defaultWorkerCoordinates; // or Location..
    int m_maxTaskSize = 100;

    int m_forkliftSize = 0;
    int m_forkliftUsedCounter = 0;
    int m_workerSize = 0;
    int m_carrierSize = 0;
    int m_carrierUsedCounter = 0;

    std::vector<std::shared_ptr<Worker>> m_workers;
    Parameters* m_parameters;
    std::shared_ptr<std::mutex> m_outputMutex;
    std::atomic<bool>* m_isEnteringCommand;

    std::thread m_schedulerThread;
    bool m_stopThread;
    std::queue<std::shared_ptr<Order>> m_orderQueue;
    std::mutex m_schedulerMutex;

    bool affectToolToWorker(const ToolType& toolType, const int& workerId);
    void affectTaskToWorker(std::shared_ptr<Task> task, const int& workerId);
    int affectTaskToIdleWorker(std::shared_ptr<Task> task);
    int getIdleWorker() const;

    std::shared_ptr<Order> fetchNextOrder();
    void storeOrder(std::shared_ptr<Order> task);
    virtual void scheduleThread() final;

    Scheduler();
public:
    ~Scheduler();
    static Scheduler& getInstance() {
        static Scheduler instance;
        return instance;
    }
    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    void runScheduler();
    std::shared_ptr<Worker> createWorker(const std::string& name, bool nameById);
    void init(int workerSize);
    void loadFromFile(const std::string& filename);
    size_t getWorkerSize() const;
    void affectOrder(std::shared_ptr<Order> order);
    void setParameters(Parameters* parameters);
    void setOutputMutex(std::shared_ptr<std::mutex> outputMutex);
    void setDefaultWorkerCoordinates(const Point2D& coordinates);
    void setMaxTaskWorkerSize(const int& maxTaskSize);
    std::shared_ptr<Worker> getWorker(const int& workerId) const;
    const std::vector<std::shared_ptr<Worker>>& getWorkers() const {
        return m_workers;
    }
    bool hasRemainingTask() const;
    bool areWorkersIdle() const;
    void stopScheduler();
    void setShowLoadingAtomic(std::atomic<bool>& isEnteringCommand) {
        m_isEnteringCommand = &isEnteringCommand;
    }
    void printStatus() {
        std::lock_guard<std::mutex> lock(m_schedulerMutex);

        std::cout << "=== Scheduler Status ===" << std::endl;

        // Workers
        std::cout << "Workers: " << m_workers.size() << std::endl;
        for (const auto& worker : m_workers) {
            std::cout << "  Worker ID: " << worker->getId()
                    << ", Name: " << worker->getName()
                    << ", Status: " << (worker->isBusy() ? "Busy" : "Idle")
                    << std::endl;
        }

        // Orders
        std::cout << "Orders in Queue: " << m_orderQueue.size() << std::endl;

        // Tools
        std::cout << "Forklifts: " << m_forkliftSize << ", In Use: " << m_forkliftUsedCounter << std::endl;
        std::cout << "Carriers: " << m_carrierSize << ", In Use: " << m_carrierUsedCounter << std::endl;

        // Tasks and Idle Workers
        std::cout << "Remaining Tasks: " << (hasRemainingTask() ? "Yes" : "No") << std::endl;
        std::cout << "Idle Workers: " << getIdleWorker() << std::endl;

        std::cout << "=========================" << std::endl;
    }
};

#endif
