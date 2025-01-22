#ifndef ACTIONNER_HPP_
#define ACTIONNER_HPP_

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <chrono>
#include <string>
#include <queue>
#include "task.hpp"
#include "../parameters.hpp"
#include "location.hpp"

class Actionner {
protected:
    int m_id;                                 // Identifiant de l'Actionner
    std::string m_name;
    std::atomic<bool> m_isBusy{false};        // Indique si l'Actionner est occupé
    std::atomic<bool> m_stopThread{false};    // Indique si le thread doit s'arrêter
    std::mutex m_taskMutex;                   // Mutex pour synchronisation
    Parameters m_config;
    Point2D m_currentCoodinates;
    std::mutex& m_outputMutex;                // Mutex global pour l'affichage
    std::thread m_workerThread;               // Thread pour l'exécution
    std::queue<std::shared_ptr<Task>> m_taskQueue;
    std::condition_variable m_taskCondition;  // Condition pour attendre une nouvelle tâche

    const int m_maxSizeQueue;

    virtual void threadLoop() {
        while (!m_stopThread) {

            std::shared_ptr<Task> currentTask = nullptr;
            if (!m_taskQueue.empty()) {
                currentTask = m_taskQueue.front();
                m_taskQueue.pop();
            }

            std::unique_lock<std::mutex> lock(m_taskMutex);
            m_taskCondition.wait(lock, [this, &currentTask]() { return currentTask != nullptr || m_stopThread; });

            if (m_stopThread) break;

            m_isBusy = true;

            std::thread loadingThread(&Actionner::loadingBar, this, currentTask);
            currentTask->execute();

            if (loadingThread.joinable()) {
                loadingThread.join();
            }

            m_isBusy = false;
        }
    }

    void printProgress(std::string taskName, float progress, int barWidth) const
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

    void loadingBar(std::shared_ptr<Task> task) const {
        const int barWidth = 50;

        float simDurationMs = static_cast<float>(task->getDuration());
        float timescale     = m_config.timescale;

        float realDurationMs = simDurationMs / timescale; 

        auto startTime = std::chrono::steady_clock::now();
        auto endTime   = startTime + std::chrono::milliseconds(static_cast<long long>(realDurationMs));

        auto frameInterval = std::chrono::milliseconds(50);
        auto nextRefresh = startTime;
    
        while (true) {
            std::this_thread::sleep_until(nextRefresh);

            auto now = std::chrono::steady_clock::now();
            if (now >= endTime) 
            {
                // La durée réelle fixée est atteinte (ou dépassée)
                // => on force la progression à 100% et on sort
                {
                    std::lock_guard<std::mutex> lock(m_outputMutex);
                    printProgress(task->getName(), 1.0f, barWidth); 
                }
                break;
            }

            auto elapsedMs = static_cast<float>(
                std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count()
            );
            
            // d) Calcul de la progression [0..1]
            float progress = elapsedMs / realDurationMs;
            if (progress > 1.0f) progress = 1.0f;

            // e) Afficher la barre
            {
                std::lock_guard<std::mutex> lock(m_outputMutex);
                printProgress(task->getName(), progress, barWidth);
            }

            // f) Incrémenter le prochain rafraîchissement
            nextRefresh += frameInterval;
        }
    }

public:
    Actionner() = default;
    Actionner(int actionnerId, const std::string& name, std::mutex& outputMtx,
              const Parameters& config = Parameters(), const Point2D& currentLocation = Point2D(),
              const int maxTaskSize = 100)
        : m_id(actionnerId), m_name(name), m_outputMutex(outputMtx),
            m_config(config), m_currentCoodinates(currentLocation),
                m_maxSizeQueue(maxTaskSize) {
        m_workerThread = std::thread(&Actionner::threadLoop, this);
    }
    Actionner(const Actionner& other) : m_id(other.m_id), m_name(other.m_name),
        m_outputMutex(other.m_outputMutex), m_config(other.m_config), 
            m_currentCoodinates(other.m_currentCoodinates),
                m_maxSizeQueue(other.m_maxSizeQueue) {}

    virtual ~Actionner() {
        m_stopThread = true;
        m_taskCondition.notify_all();
        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
    }

    bool submitTask(std::shared_ptr<Task> task) {
        if (m_taskQueue.size() < m_maxSizeQueue) {
            bool isEmpty = m_taskQueue.empty();
            m_taskQueue.push(task);

            if (isEmpty) m_taskCondition.notify_one();
            return true;
        }
        return false;
    }

    bool busy() const {
        return m_isBusy.load();
    }

    void setConfig(const Parameters& config) {
        m_config = config;
    }

    void setCoordinates(const Point2D& coordinates) { m_currentCoodinates = coordinates; }
    void setCoordinates(const double& x, const double& y) { 
        m_currentCoodinates.x = x;
        m_currentCoodinates.y = y;
    }
    const Point2D& getCoordinates() const { return m_currentCoodinates; }
};

#endif
