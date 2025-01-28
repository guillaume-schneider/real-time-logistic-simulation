#include "scheduler.hpp"
#include "task_factory.hpp"


std::atomic<int> Scheduler::m_workerCounter{0};


std::shared_ptr<Worker> Scheduler::createWorker(const std::string& name, bool nameById = false) {
    auto workerName = name;
    m_workerCounter++;
    if (nameById) workerName += " " + std::to_string(m_workerCounter);
    auto worker = std::make_shared<Worker>(m_workerCounter, workerName, m_outputMutex, *m_isEnteringCommand, m_parameters,
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
        if (!worker->hasTask() && !worker->isBusy()) {
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

void Scheduler::affectOrder(std::shared_ptr<Order> order) {
    int idleWorkerId = getIdleWorker();
    if (idleWorkerId < 0) {
        m_orderQueue.emplace(order);
        return;
    }
    auto idleWorker = getWorker(idleWorkerId);
    auto prepareTask = TaskFactory::createPrepareOrderTask(order, idleWorker);
    affectTaskToWorker(prepareTask, idleWorkerId);
}


std::shared_ptr<Order> Scheduler::fetchNextOrder() {
    std::lock_guard<std::mutex> lock(m_schedulerMutex);
    if (!m_orderQueue.empty()) {
        auto order = m_orderQueue.front();
        m_orderQueue.pop();
        return order;
    }
    return nullptr;
}


void Scheduler::storeOrder(std::shared_ptr<Order> order) {
    std::lock_guard<std::mutex> lock(m_schedulerMutex);
    m_orderQueue.push(order);
}


void Scheduler::scheduleThread() {
    while (!m_stopThread) {
        std::this_thread::sleep_for(std::chrono::milliseconds(refreshThreadTimeInMs));

        std::shared_ptr<Order> order = fetchNextOrder();
        if (order) {
            int idleWorkerId = getIdleWorker();
            if (idleWorkerId != -1) {
                auto idleWorker = getWorker(idleWorkerId);
                auto prepareTask = TaskFactory::createPrepareOrderTask(order, idleWorker);
                affectTaskToWorker(prepareTask, idleWorkerId);            
            } else {
                storeOrder(order);
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
    m_orderQueue = std::queue<std::shared_ptr<Order>>();
}

bool Scheduler::hasRemainingTask() const {
    return !m_orderQueue.empty();
}

bool Scheduler::areWorkersIdle() const {
    for (auto worker : m_workers) {
        if (worker->hasTask() || worker->isBusy()) {
            return false;
        }
    }
    return true;
}
