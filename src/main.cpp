#include <iostream>
#include <thread>
#include <chrono>

void loadingBarAsync(int totalSteps, int delayMilliseconds) {
    const int barWidth = 50;
    for (int step = 0; step <= totalSteps; ++step) {
        float progress = static_cast<float>(step) / totalSteps;
        std::cout << "[";
        int pos = static_cast<int>(barWidth * progress);
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
    }
    std::cout << std::endl;
}

void backgroundTask() {
    // Exemple d'une tâche en arrière-plan
    std::this_thread::sleep_for(std::chrono::seconds(5)); // Simule une tâche de 5 secondes
}

int main() {
    std::thread loadingThread(loadingBarAsync, 100, 50);
    backgroundTask(); // Effectue une autre tâche pendant le chargement
    loadingThread.join(); // Attend que la barre de chargement se termine
    std::cout << "Tâche terminée !" << std::endl;
    return 0;
}
