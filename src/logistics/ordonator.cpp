#include "ordonator.hpp"
#include "task_factory.hpp"


std::atomic<int> Ordonator::m_workerCounter{0};

Ordonator::Ordonator()
    : m_parameters(nullptr), m_outputMutex(nullptr), m_maxTaskSize(100) {}

std::shared_ptr<Worker> Ordonator::createWorker(const std::string& name, bool nameById = false) {
    auto workerName = name;
    m_workerCounter++;
    if (nameById) workerName += " " + std::to_string(m_workerCounter);
    auto worker = std::make_shared<Worker>(m_workerCounter, workerName, m_outputMutex, m_parameters,
                            m_defaultWorkerCoordinates, m_maxTaskSize);
        
    m_workers.push_back(worker);
    return worker;
}

void Ordonator::init(int workerSize) {
    for (int i = 0; i < workerSize; i++) {
        createWorker("Worker", true);
    }
}

void Ordonator::loadFromFile(const std::string& filename) {
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

        init(m_workerSize);
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }
}

bool Ordonator::affectToolToWorker(const ToolType& toolType, const int& workerId) {
    for (auto& worker : m_workers) {
        if (worker->getId() == workerId) {
            switch (toolType) {
                case ToolType::Forklift:
                    if (m_forkliftUsedCounter >= m_forkliftSize) {
                        std::cerr << "Ordonator: all the forlift are being used.\n";
                        return false;
                    } else {
                        worker->setTool(toolType);
                        m_forkliftUsedCounter++;
                        return true;
                    }
                    break;
                case ToolType::Carrier:
                    if (m_carrierUsedCounter >= m_carrierSize) {
                        std::cerr << "Ordonator: all the carrier are being used.\n";
                        return false;
                    } else {
                        worker->setTool(toolType);
                        m_carrierUsedCounter++;
                        return true;
                    }
                    break;
            }
            worker->setTool(toolType);
        }
        return false;
    }
    return false;
}

void Ordonator::affectTaskToWorker(std::shared_ptr<Task> task, const int& workerId) {
    for (auto& worker : m_workers) {
        if (worker->getId() == workerId) {
            worker->submitTask(task);
        }
    }
}

int Ordonator::affectTaskToIdleWorker(std::shared_ptr<Task> task) {
    int workerId = -1;
    for (auto& worker : m_workers) {
        if (!worker->hasTask()) {
            workerId = worker->getId();
            worker->submitTask(task);
            return workerId;
        }
    }
    return workerId;
}

int Ordonator::getIdleWorker() const {
    int workerId = -1;
    for (auto& worker : m_workers) {
        if (!worker->hasTask()) {
            workerId = worker->getId();
        }
    }
    return workerId;
}

size_t Ordonator::getWorkerSize() const {
    return m_workers.size();
}

void Ordonator::setParameters(Parameters* parameters) {
    m_parameters = parameters;
}

void Ordonator::setOutputMutex(std::mutex* outputMutex) {
    m_outputMutex = outputMutex;
}

void Ordonator::setDefaultWorkerCoordinates(const Point2D& coordinates) {
    m_defaultWorkerCoordinates = coordinates;
}

void Ordonator::setMaxTaskWorkerSize(const int& maxTaskSize) {
    m_maxTaskSize = maxTaskSize;
}

std::shared_ptr<Worker> Ordonator::getWorker(const int& workerId) const {
    for (const auto& worker : m_workers) {
        if (worker->getId() == workerId)
            return worker;
    }
    return nullptr;
}

void Ordonator::affectOrder(const Order& order) {
    int idleWorkerId = getIdleWorker();
    auto idleWorker = getWorker(idleWorkerId);
    auto prepareTask = TaskFactory::createPrepareOrderTask(order, idleWorker);
    affectTaskToWorker(prepareTask, idleWorkerId);
}
