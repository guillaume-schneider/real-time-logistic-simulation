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
private:
    int m_id;                                 // Identifiant de l'Actionner
    std::string m_name;
    std::thread m_workerThread;               // Thread pour l'exécution
    std::atomic<bool> m_isBusy{false};        // Indique si l'Actionner est occupé
    std::atomic<bool> m_stopThread{false};    // Indique si le thread doit s'arrêter
    std::mutex m_taskMutex;                   // Mutex pour synchronisation
    std::condition_variable m_taskCondition;  // Condition pour attendre une nouvelle tâche
    std::condition_variable m_doneCondition;  // Condition pour attendre la fin d'une tâche
    Config m_config;
    Location m_currentLocation;
    std::queue<Task> m_taskQueue;

    Task m_currentTask;
    std::mutex& m_outputMutex;                // Mutex global pour l'affichage

    void threadLoop() {
        while (!m_stopThread) {

            if (m_taskQueue.size() > 0 && m_currentTask.hasExecuted()) {
                m_currentTask = m_taskQueue.front();
                m_taskQueue.pop();
            }

            std::unique_lock<std::mutex> lock(m_taskMutex);
            m_taskCondition.wait(lock, [this]() { return !m_currentTask.getActions().empty() 
                && !m_currentTask.hasExecuted() || m_stopThread; });

            if (m_stopThread) break;

            // if (!m_currentTask.getActions().empty() && !m_currentTask.hasExecuted()) {
                m_isBusy = true;

                std::thread loadingThread(&Actionner::loadingBar, this);
                m_currentTask.execute();

                if (loadingThread.joinable()) {
                    loadingThread.join();
                }

                m_isBusy = false;

                // Notify that the task is complete
                m_doneCondition.notify_all();
            // }
        }
    }

    void printProgress(float progress, int barWidth) const
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
                << " - " << m_currentTask.getName() << "   \r";
        std::cout.flush();
    }

    void loadingBar() const {
        const int barWidth = 50;

        float simDurationMs = static_cast<float>(m_currentTask.getDuration());
        float timescale     = m_config.timescale;
        if (timescale <= 0.0f) {
            timescale = 0.0001f;
        }

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
                    printProgress(1.0f, barWidth); 
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
                printProgress(progress, barWidth);
            }

            // f) Incrémenter le prochain rafraîchissement
            nextRefresh += frameInterval;
        }
    }

public:
    Actionner() = default;
    Actionner(int actionnerId, const std::string& name, std::mutex& outputMtx,
              const Config& config = Config(), const Location& currentLocation = Location())
        : m_id(actionnerId), m_name(name), m_outputMutex(outputMtx), m_config(config), m_currentLocation(currentLocation) {
        m_workerThread = std::thread(&Actionner::threadLoop, this);
    }
    Actionner(const Actionner& other) : m_id(other.m_id), m_name(other.m_name),
        m_outputMutex(other.m_outputMutex), m_config(other.m_config), 
            m_currentLocation(other.m_currentLocation) {}

    ~Actionner() {
        m_stopThread = true;
        m_taskCondition.notify_all();
        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
    }

    Actionner& operator=(const Actionner& other) {
        if (&other == this) return *this;
        m_id = other.m_id;
        m_name = other.m_name;
        m_config = other.m_config;
        m_currentLocation = other.m_currentLocation;
        return *this;
    }

    bool submitTask(Task task) {
        if (!m_currentTask.hasExecuted() && !m_currentTask.getActions().empty()) {
            std::cerr << "Actionner " << m_id << " is currently busy.\n";
            m_taskQueue.push(task);
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(m_taskMutex);
            m_currentTask = task;
        }
        m_taskCondition.notify_one();
        return true;
    }

    bool busy() const {
        return m_isBusy.load();
    }

    void waitUntilDone() {
        std::unique_lock<std::mutex> lock(m_taskMutex);
        m_doneCondition.wait(lock, [this]() { return !m_isBusy; });
    }

    void setConfig(const Config& config) {
        m_config = config;
    }

    void setLocation(const Location& location) { m_currentLocation = location; }
    Location& getLocation() { return m_currentLocation; }
};

#endif
