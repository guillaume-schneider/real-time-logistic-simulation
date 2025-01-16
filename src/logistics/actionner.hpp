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

class Actionner {
private:
    int id;                                 // Identifiant de l'Actionner (ligne assignée)
    std::string m_name;
    std::thread workerThread;               // Thread pour l'exécution
    std::atomic<bool> isBusy{false};        // Indique si l'Actionner est occupé
    std::atomic<bool> stopThread{false};    // Indique si le thread doit s'arrêter
    std::mutex taskMutex;                   // Mutex pour synchronisation
    std::condition_variable taskCondition;  // Condition pour attendre une nouvelle tâche

    std::function<void()> currentTask;      // La tâche actuelle à exécuter
    int taskDuration = 0;                   // Durée de la tâche en millisecondes
    std::string m_taskDescription;
    std::mutex& outputMutex;                // Mutex global pour l'affichage

    // Méthode exécutée par le thread
    void threadLoop() {
        while (!stopThread) {
            std::unique_lock<std::mutex> lock(taskMutex);
            taskCondition.wait(lock, [this]() { return currentTask || stopThread; });

            if (stopThread) break;

            if (currentTask) {
                isBusy = true; // Marquer comme occupé

                // Lancer la barre de chargement
                std::thread loadingThread(&Actionner::loadingBar, this, taskDuration);
                currentTask(); // Exécuter la tâche

                // Fin de la tâche
                isBusy = false;
                currentTask = nullptr;

                // Attendre la fin de la barre de chargement
                if (loadingThread.joinable()) {
                    loadingThread.join();
                }
            }
        }
    }

    // Afficher une barre de chargement sur une ligne spécifique
    void loadingBar(int duration) const {
        const int barWidth = 50;
        auto startTime = std::chrono::steady_clock::now();

        while (true) {
            auto now = std::chrono::steady_clock::now();
            float elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

            float progress = elapsedTime / duration;
            progress = std::min(progress, 1.0f);

            std::string bar = "[";
            int pos = static_cast<int>(barWidth * progress);
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) bar += "=";
                else if (i == pos) bar += ">";
                else bar += " ";
            }
            bar += "] " + std::to_string(int(progress * 100)) + "%";

            // Afficher la barre de chargement avec le nom
            {
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cout << "\33[" << (id + 1) << ";1H" << m_name << ": " << bar << " - " << m_taskDescription << "   \r";
                std::cout.flush();
            }

            if (progress >= 1.0f) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "\33[" << (id + 1) << ";1H" << m_name << ": [";
            for (int i = 0; i < barWidth; ++i) std::cout << "=";
            std::cout << "] 100% - Task Complete!\n" << std::flush;
        }
    }

public:
    Actionner(int actionnerId, const std::string& name, std::mutex& outputMtx)
        : id(actionnerId), m_name(name), outputMutex(outputMtx) {
        // Lancer le thread de travail
        workerThread = std::thread(&Actionner::threadLoop, this);
    }

    ~Actionner() {
        stopThread = true;
        taskCondition.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }

    // Soumettre une tâche à l'Actionner
    bool submitTask(std::function<void()> task, int duration, std::string description) {
        if (isBusy) {
            std::cerr << "Actionner " << id << " is currently busy.\n";
            return false;
        }

        {
            std::lock_guard<std::mutex> lock(taskMutex);
            currentTask = std::move(task);
            taskDuration = duration;
            m_taskDescription = description;
        }
        taskCondition.notify_one();
        return true;
    }

    // Vérifier si l'Actionner est occupé
    bool busy() const {
        return isBusy.load();
    }
};

#endif // ACTIONNER_HPP_
