#include "scheduler.hpp"
#include "task_factory.hpp"


std::atomic<int> Scheduler::m_workerCounter{0};


std::shared_ptr<Worker> Scheduler::createWorker(const std::string& name, bool nameById = false) {
    auto workerName = name;
    m_workerCounter++;
    if (nameById) workerName += " " + std::to_string(m_workerCounter);
    auto worker = std::make_shared<Worker>(m_workerCounter, workerName, m_outputMutex, m_parameters,
                            m_defaultWorkerCoordinates, m_maxTaskSize);
        
    m_workers.push_back(worker);
    return worker;
}

void Scheduler::init(int workerSize) {
    for (int i = 0; i < workerSize; i++) {
        createWorker("Worker", true);
    }
}

Scheduler::~Scheduler() {
    m_stopThread = true;
    if (m_schedulerThread.joinable()) {
        m_schedulerThread.join();
    }
}

Scheduler::Scheduler() 
    : m_parameters(nullptr), m_outputMutex(nullptr), m_maxTaskSize(100) {}

void Scheduler::runScheduler() {
    m_schedulerThread = std::thread(&Scheduler::scheduleThread, this);
}

void Scheduler::loadFromFile(const std::string& filename) {
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

bool Scheduler::affectToolToWorker(const ToolType& toolType, const int& workerId) {
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

void Scheduler::affectTaskToWorker(std::shared_ptr<Task> task, const int& workerId) {
    for (auto& worker : m_workers) {
        if (worker->getId() == workerId) {
            worker->submitTask(task);
        }
    }
}

int Scheduler::affectTaskToIdleWorker(std::shared_ptr<Task> task) {
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

int Scheduler::getIdleWorker() const {
    int workerId = -1;
    for (auto& worker : m_workers) {
        if (!worker->hasTask()) {
            workerId = worker->getId();
        }
    }
    return workerId;
}

size_t Scheduler::getWorkerSize() const {
    return m_workers.size();
}

void Scheduler::setParameters(Parameters* parameters) {
    m_parameters = parameters;
}

void Scheduler::setOutputMutex(std::shared_ptr<std::mutex> outputMutex) {
    m_outputMutex = outputMutex;
}

void Scheduler::setDefaultWorkerCoordinates(const Point2D& coordinates) {
    m_defaultWorkerCoordinates = coordinates;
}

void Scheduler::setMaxTaskWorkerSize(const int& maxTaskSize) {
    m_maxTaskSize = maxTaskSize;
}

std::shared_ptr<Worker> Scheduler::getWorker(const int& workerId) const {
    for (const auto& worker : m_workers) {
        if (worker->getId() == workerId)
            return worker;
    }
    return nullptr;
}

void Scheduler::affectOrder(const Order& order) {
    int idleWorkerId = getIdleWorker();
    if (idleWorkerId < 0) {

    }
    auto idleWorker = getWorker(idleWorkerId);
    auto prepareTask = TaskFactory::createPrepareOrderTask(order, idleWorker);
    affectTaskToWorker(prepareTask, idleWorkerId);
}


std::shared_ptr<Task> Scheduler::fetchNextTask() {
    std::lock_guard<std::mutex> lock(m_schedulerMutex);
    if (!m_taskQueue.empty()) {
        auto task = m_taskQueue.front();
        m_taskQueue.pop();
        return task;
    }
    return nullptr;
}


void Scheduler::storeTask(std::shared_ptr<Task> task) {
    std::lock_guard<std::mutex> lock(m_schedulerMutex);
    m_taskQueue.push(task);
}


void Scheduler::scheduleThread() {
    while (!m_stopThread) {
        std::this_thread::sleep_for(std::chrono::milliseconds(refreshThreadTimeInMs));

        std::shared_ptr<Task> task = fetchNextTask();
        if (task) {
            int idleWorkerId = getIdleWorker();
            if (idleWorkerId != -1) {
                affectTaskToWorker(task, idleWorkerId);
            } else {
                storeTask(task);
            }
        }
    }
}

void Scheduler::stopScheduler() {
    m_stopThread = true;
    if (m_schedulerThread.joinable()) {
        m_schedulerThread.join();
    }
    m_workers.clear();
    m_taskQueue = std::queue<std::shared_ptr<Task>>();
}
