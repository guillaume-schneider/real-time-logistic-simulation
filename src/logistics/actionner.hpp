#ifndef ACTIONNER_HPP_
#define ACTIONNER_HPP_

#include <mutex>
#include <functional>
#include <condition_variable>
#include <string>
#include <queue>
#include "task.hpp"
#include "../parameters.hpp"
#include "coordinates.hpp"
#include <memory>
#include <atomic>


class Actionner {
protected:
    int m_id;                                 // Identifiant de l'Actionner
    std::string m_name;
    std::atomic<bool> m_isBusy{false};        // Indique si l'Actionner est occupé
    std::atomic<bool> m_stopThread{false};    // Indique si le thread doit s'arrêter
    std::mutex m_taskMutex;                   // Mutex pour synchronisation
    Parameters* m_parameters;
    std::shared_ptr<std::mutex> m_outputMutex;                // Mutex global pour l'affichage
    std::thread m_workerThread;               // Thread pour l'exécution
    std::queue<std::shared_ptr<Task>> m_taskQueue;
    std::condition_variable m_taskCondition;  // Condition pour attendre une nouvelle tâche

    const int m_maxSizeQueue;

    virtual void threadLoop() final;
    void printProgress(std::string taskName, float progress, int barWidth) const;
    void loadingBar(std::shared_ptr<Task> task) const;

public:
    Actionner();
    Actionner(int actionnerId, const std::string& name, std::shared_ptr<std::mutex> outputMtx,
              Parameters* parameters, const int maxTaskSize);
    Actionner(const Actionner& other);
    virtual ~Actionner();
    bool submitTask(std::shared_ptr<Task> task);
    bool isBusy() const;
    bool hasTask() const;
    void setParameters(Parameters* parameters);
};

#endif
