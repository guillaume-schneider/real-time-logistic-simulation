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
#include <atomic>

class Ordonator {
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
    std::mutex* m_outputMutex;

    bool affectToolToWorker(const ToolType& toolType, const int& workerId);
    void affectTaskToWorker(std::shared_ptr<Task> task, const int& workerId);
    int affectTaskToIdleWorker(std::shared_ptr<Task> task);
    int getIdleWorker() const;

    Ordonator();
public:
    static Ordonator& getInstance() {
        static Ordonator instance;
        return instance;
    }
    Ordonator(const Ordonator&) = delete;
    Ordonator& operator=(const Ordonator&) = delete;

    std::shared_ptr<Worker> createWorker(const std::string& name, bool nameById);
    void init(int workerSize);
    void loadFromFile(const std::string& filename);
    size_t getWorkerSize() const;
    void affectOrder(const Order& order);
    void setParameters(Parameters* parameters);
    void setOutputMutex(std::mutex* outputMutex);
    void setDefaultWorkerCoordinates(const Point2D& coordinates);
    void setMaxTaskWorkerSize(const int& maxTaskSize);
    std::shared_ptr<Worker> getWorker(const int& workerId) const;
    const std::vector<std::shared_ptr<Worker>>& getWorkers() const {
        return m_workers;
    }
};

#endif
