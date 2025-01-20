#ifndef CONVEYOR_HPP_
#define CONVEYOR_HPP_

#include "actionner.hpp"
#include <vector>
#include <chrono>

class Conveyor : public Actionner {
private:
    struct ActiveTask {
        Task     task;
        float    progress;         // Current progress [0..1]
        float    realDurationMs;   // Computed duration in real time (simulation scaled)
        bool     done;

        ActiveTask(const Task& t, float duration)
            : task(t), progress(0.0f), realDurationMs(duration), done(false) {}
    };

    // A list of tasks currently processed "in parallel"
    std::vector<ActiveTask> m_activeTasks;
    bool m_stopThread = false;

    // We override the thread loop to handle multiple tasks simultaneously
    void threadLoop() override {
        using namespace std::chrono;

        const auto frameInterval = milliseconds(50);

        while (!m_stopThread) {
            // Move any new tasks from the parent's queue into m_activeTasks
            loadNewTasks();

            if (m_activeTasks.empty()) {
                // If no active tasks, go to sleep until a new task arrives or stop is requested
                std::unique_lock<std::mutex> lock(m_taskMutex);
                m_taskCondition.wait_for(lock, frameInterval, [this]{
                    return !this->m_taskQueue.empty() || m_stopThread;
                });
                continue; // then loop again
            }

            // Mark the conveyor as busy if we have active tasks
            m_isBusy = true;

            // Update all active tasks (simulate the conveyor's "movement")
            auto startCycle = steady_clock::now();
            std::this_thread::sleep_until(startCycle + frameInterval); // throttle refresh rate

            // Lock for safe access to tasks & printing
            {
                std::lock_guard<std::mutex> lock(m_outputMutex);

                // Update each active task's progress
                for (size_t i = 0; i < m_activeTasks.size(); i++) {
                    auto& active = m_activeTasks[i];
                    if (!active.done) {
                        // Increment progress
                        float dt = static_cast<float>(
                            duration_cast<milliseconds>(frameInterval).count()
                        );
                        active.progress += dt / active.realDurationMs;
                        if (active.progress >= 1.0f) {
                            active.progress = 1.0f;
                            active.done     = true;
                            // Execute the task (simulating final action)
                            active.task.execute();
                        }
                    }
                }

                // Print progress bars for all tasks
                for (size_t i = 0; i < m_activeTasks.size(); i++) {
                    printProgressBar(m_activeTasks[i], i);
                }
            }

            // Remove finished tasks
            removeFinishedTasks();

            // If no tasks remain active, we can signal that the conveyor is not busy
            if (m_activeTasks.empty() && m_taskQueue.empty()) {
                m_isBusy = false;
                // Notify any thread waiting for tasks to complete (like waitUntilDone())
                m_doneCondition.notify_all();
            }
        } // end while

        // Cleanup: If we're stopping, we might want to finalize any tasks in the queue
        // or simply ignore them. For simplicity, we do nothing special here.
    }

    // Helper: Move newly submitted tasks from the parent queue to our active list
    void loadNewTasks() {
        std::lock_guard<std::mutex> lock(m_taskMutex);

        while (!m_taskQueue.empty()) {
            Task next = m_taskQueue.front();
            m_taskQueue.pop();

            // Calculate how long this task should take in real time (based on parent's config)
            float simDurationMs = static_cast<float>(next.getDuration());
            float timescale     = m_config.timescale;
            if (timescale <= 0.0f) {
                timescale = 0.0001f;
            }
            float realDurationMs = simDurationMs / timescale;

            m_activeTasks.emplace_back(next, realDurationMs);
        }
    }

    // Helper: Remove tasks whose progress is fully done
    void removeFinishedTasks() {
        auto it = std::remove_if(m_activeTasks.begin(), m_activeTasks.end(),
                                 [](const ActiveTask& at) { return at.done; });
        m_activeTasks.erase(it, m_activeTasks.end());
    }

    // Print a single task's progress bar on a distinct line
    void printProgressBar(const ActiveTask& active, size_t index) const {
        const int barWidth = 50;

        // Build the bar
        int pos = static_cast<int>(barWidth * active.progress);
        std::string bar = "[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos)      bar += "=";
            else if (i == pos) bar += ">";
            else              bar += " ";
        }
        bar += "] " + std::to_string(int(active.progress * 100)) + "%";

        // We use the parent's ID (m_id) as a base row, plus the index for each task
        int row = m_id + 1 + static_cast<int>(index);

        // Clear the line and print progress
        std::cout << "\33[" << row << ";1H"         // Move cursor to "row", column 1
                  << m_name << " (Task " << index << "): " << bar
                  << " - " << active.task.getName() << "   \r";
        std::cout.flush();
    }

public:
    // Inherit the parent constructor(s)
    using Actionner::Actionner;

    // Destructor
    ~Conveyor() override {
        // Signal stop and join the thread
        m_stopThread = true;
        if (m_workerThread.joinable()) {
            m_workerThread.join();
        }
    }
};

#endif // CONVEYOR_HPP_
