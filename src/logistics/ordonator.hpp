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
#include <optional>

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

    std::vector<Worker> m_workers;
    Parameters* m_parameters;
    std::mutex* m_outputMutex;

    Ordonator()
        : m_parameters(nullptr), m_outputMutex(nullptr), m_maxTaskSize(100) {}
public:
    static Ordonator& getInstance() {
        static Ordonator instance;
        return instance;
    }
    Ordonator(const Ordonator&) = delete;
    Ordonator& operator=(const Ordonator&) = delete;

    Worker createWorker(const std::string& name, bool nameById = false) {
        auto workerName = name;
        m_workerCounter++;
        if (nameById) workerName += " " + std::to_string(m_workerCounter);
        auto worker = Worker(m_workerCounter, workerName, *m_outputMutex, *m_parameters,
                             m_defaultWorkerCoordinates, m_maxTaskSize);
        m_workers.push_back(worker);
        return worker;
    }

    void init() {
        for (int i = 0; i < m_workerSize; i++) {
            createWorker("Worker", true);
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ordonator Error: Could not open file " << filename << std::endl;
            return;
        }

        try {
            nlohmann::json jsonData;
            file >> jsonData;

            m_workerSize = jsonData.at("worker").get<int>();
            m_forkliftSize = jsonData.at("forklift").get<int>();
            m_carrierSize = jsonData.at("carrier").get<int>();

            init();
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            return;
        }
    }

    bool affectToolToWorker(const ToolType& toolType, const int& workerId) {
        for (auto& worker : m_workers) {
            if (worker.getId() == workerId) {
                switch (toolType) {
                    case ToolType::Forklift:
                        if (m_forkliftUsedCounter >= m_forkliftSize) {
                            std::cerr << "Ordonator: all the forlift are being used.\n";
                            return false;
                        } else {
                            worker.setTool(toolType);
                            m_forkliftUsedCounter++;
                            return true;
                        }
                        break;
                    case ToolType::Carrier:
                        if (m_carrierUsedCounter >= m_carrierSize) {
                            std::cerr << "Ordonator: all the carrier are being used.\n";
                            return false;
                        } else {
                            worker.setTool(toolType);
                            m_carrierUsedCounter++;
                            return true;
                        }
                        break;
                }
                worker.setTool(toolType);
            }
        }
    }

    void setParameters(Parameters* parameters) {
        m_parameters = parameters;
    }

    void setOutputMutex(std::mutex* outputMutex) {
        m_outputMutex = outputMutex;
    }

    void setDefaultWorkerCoordinates(const Point2D& coordinates) {
        m_defaultWorkerCoordinates = coordinates;
    }

    void setMaxTaskWorkerSize(const int& maxTaskSize) {
        m_maxTaskSize = maxTaskSize;
    }

    std::optional<Worker> getWorker(const int& workerId) {
        for (const auto& worker : m_workers) {
            if (worker.getId() == workerId)
                return worker;
        }
        return std::nullopt;
    }

    std::vector<Worker> getWorkers() {
        return m_workers;
    }
};

#endif
