
#include "actionner.hpp"
#include <chrono>
#include <atomic>
#include <thread>
#include <iostream>


void Actionner::threadLoop() {
   while (!m_stopThread) {
        std::shared_ptr<Task> currentTask = nullptr;

        {
            std::unique_lock<std::mutex> lock(m_taskMutex);

            if (m_taskQueue.empty()) {
                m_taskCondition.wait(lock, [this]() { return !m_taskQueue.empty() || m_stopThread; });
            }

            if (m_stopThread) {
                break;
            }

            if (!m_taskQueue.empty()) {
                currentTask = m_taskQueue.front();
                m_taskQueue.pop();
                m_isBusy = true;
            }
        }

        if (!currentTask) continue;

        std::thread loadingThread(&Actionner::loadingBar, this, currentTask);
        currentTask->execute();

        if (loadingThread.joinable()) {
            loadingThread.join();
        }

        m_isBusy = false;
    }
}

void Actionner::printProgress(std::string taskName, float progress, int barWidth) const
{
    // Construction de la barre
    int pos = static_cast<int>(barWidth * progress);
    std::string bar = "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)      bar += "=";
        else if (i == pos) bar += ">";
        else              bar += " ";
    }
    bar += "] " + std::to_string(int(progress * 100)) + "%";

    // Affichage
    std::cout << "\33[" << (m_id + 1) << ";1H"
            << m_name << ": " << bar
            << " - " << taskName << "   \r";
    std::cout.flush();
}

long long formatTime(long long time, float timescale) {
    return static_cast<long long>(time / timescale);
}


#include "../utils.hpp"


void Actionner::loadingBar(std::shared_ptr<Task> task) const {
    auto actionsDescription = task->getActionDescriptions();
    const int barWidth = 50;

    float simDurationMs = static_cast<float>(task->getDuration());
    float timescale     = m_parameters->timescale;

    float realDurationMs = simDurationMs / timescale; 

    auto startTime = std::chrono::steady_clock::now();
    auto endTime   = startTime + std::chrono::milliseconds(static_cast<long long>(realDurationMs));

    auto frameInterval = std::chrono::milliseconds(50);
    auto nextRefresh = startTime;
    auto currentActionName = task->getName() +  " (" + actionsDescription[0].name + ")";

    while (true) {
        std::this_thread::sleep_until(nextRefresh);
        auto now = std::chrono::steady_clock::now();
        auto elapsedMs = static_cast<float>(
            std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count()
        );

        if (!m_showLoading->load()) {
            std::lock_guard<std::mutex> lock(*m_outputMutex);
            if (!m_isLoadingDisabled) {
                moveCursor(m_id + 1, 1);
                clearLine();
                std::cout.flush();
                m_isLoadingDisabled = true;
                moveCursorToCommandLine();
            }
            continue;
        } else {
            m_isLoadingDisabled = false;
        }

        if (now >= endTime) 
        {
            {
                std::lock_guard<std::mutex> lock(*m_outputMutex);
                printProgress(currentActionName, 1.0f, barWidth);
                moveCursorToCommandLine();
            }
            break;
        }

        auto lastName = currentActionName;
        if (formatTime(actionsDescription[0].cumulatedProcessingTime, timescale) > elapsedMs) {
            currentActionName = actionsDescription[0].name;
        } else if (elapsedMs > formatTime(actionsDescription[actionsDescription.size() - 2].cumulatedProcessingTime, timescale)) {
            currentActionName = actionsDescription[actionsDescription.size() - 1].name;
        } else {
            for (int i = 0; i < actionsDescription.size() - 2; i++) {
                if (formatTime(actionsDescription[i + 1].cumulatedProcessingTime, timescale) > elapsedMs
                    && elapsedMs >= formatTime(actionsDescription[i].cumulatedProcessingTime, timescale)) {
                        currentActionName = actionsDescription[i + 1].name;
                        break;
                }
            }
        }

        if (lastName != currentActionName) {
            {
                std::lock_guard<std::mutex> lock(*m_outputMutex);
                std::cout << "\33[2K";
            }
        }
    
        float progress = elapsedMs / realDurationMs;
        if (progress > 1.0f) progress = 1.0f;
    
        {
            std::lock_guard<std::mutex> lock(*m_outputMutex);
            printProgress(currentActionName, progress, barWidth);
            moveCursorToCommandLine();
        }

        nextRefresh += frameInterval;
    }

}

Actionner::Actionner() : m_id(), m_name(), m_outputMutex(nullptr),
        m_parameters(), m_maxSizeQueue()  {
    m_workerThread = std::thread(&Actionner::threadLoop, this);
}
Actionner::Actionner(int actionnerId, const std::string& name, std::shared_ptr<std::mutex> outputMtx,
    std::atomic<bool>& isEnteringCommand, Parameters* parameters = nullptr, const int maxTaskSize = 100)
    : m_id(actionnerId), m_name(name), m_outputMutex(outputMtx),
        m_parameters(parameters), m_maxSizeQueue(maxTaskSize),
            m_showLoading(&isEnteringCommand) {
    m_workerThread = std::thread(&Actionner::threadLoop, this);
}
Actionner::Actionner(const Actionner& other) : m_id(other.m_id), m_name(other.m_name),
    m_outputMutex(other.m_outputMutex), m_parameters(other.m_parameters), 
        m_maxSizeQueue(other.m_maxSizeQueue), m_showLoading(other.m_showLoading) {
    m_workerThread = std::thread(&Actionner::threadLoop, this);
}

Actionner::~Actionner() {
    m_stopThread = true;
    m_taskCondition.notify_all();
    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

bool Actionner::submitTask(std::shared_ptr<Task> task) {
    if (m_taskQueue.size() < m_maxSizeQueue) {
        bool isEmpty = m_taskQueue.empty();
        m_taskQueue.push(task);

        m_taskCondition.notify_one();
        return true;
    }
    return false;
}

bool Actionner::hasTask() const {
    return !m_taskQueue.empty();
}

bool Actionner::isBusy() const {
    return m_isBusy.load();
}

void Actionner::setParameters(Parameters* parameters) {
    m_parameters = parameters;
}
