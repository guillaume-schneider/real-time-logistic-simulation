#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <iostream>
#include <sstream>
#include "logistics/scheduler.hpp"
#include "utils.hpp"

class Shell {
private:
    std::atomic<bool> m_stopShell;
    std::atomic<bool> m_showLoadingBars;
    std::shared_ptr<std::mutex> m_outputMutex;
    std::string m_currentCommand;
    std::vector<std::string> m_commandHistory;
    size_t m_historyIndex;

    void clearLine() const {
        std::cout << "\33[2K\r";
    }

    std::vector<std::string> splitCommand(const std::string& command) const {
        std::istringstream iss(command);
        std::vector<std::string> parts;
        std::string part;
        while (iss >> part) {
            parts.push_back(part);
        }
        return parts;
    }

    void handleCommand(const std::string& command) {
        if (command.empty()) return;

        auto parts = splitCommand(command);
        if (parts.empty()) return;

        const std::string& cmd = parts[0];

        if (cmd == "exit") {
            m_stopShell.store(true);
        } else if (cmd == "status") {
            Scheduler::getInstance().printStatus();
        } else if (cmd == "disable" && parts.size() == 2 && parts[1] == "loading") {
            m_showLoadingBars.store(false);
            std::cout << "Loading activities disabled.\n";
        } else if (cmd == "enable" && parts.size() == 2 && parts[1] == "loading") {
            m_showLoadingBars.store(true);
            std::cout << "Loading activities enabled.\n";
        } else if (cmd == "list" && parts.size() >= 2) {
            handleListCommand(parts);
        } else if (cmd == "clear") {
            clearScreen();
        } else if (cmd == "help") {
            printHelp();
        } else {
            std::cout << "Unknown command: " << command << "\n";
        }
    }

    void handleListCommand(const std::vector<std::string>& parts) {
        const std::string& subcommand = parts[1];
        if (subcommand == "products") {
            if (parts.size() > 2) {
                std::cout << "Error: 'list products' does not take any arguments.\n";
            } else {
                Site::getInstance().storage.listProducts();
            }
        } else if (subcommand == "product" && parts.size() == 3) {
            const std::string& productId = parts[2];
            auto product = Site::getInstance().storage.getProduct(productId);
            if (product) {
                std::cout << product->toString() << "\n";
            } else {
                std::cout << "Product not found: " << productId << "\n";
            }
        } else if (subcommand == "orders") {
            if (parts.size() > 2) {
                std::cout << "Error: 'list orders' does not take any arguments.\n";
            } else {
                OrderDatabase::getInstance().printAllOrders();
            }
        } else if (subcommand == "order" && parts.size() == 3) {
            const std::string& orderId = parts[2];
            auto order = OrderDatabase::getInstance().getOrder(orderId);
            if (order) {
                std::cout << order->toString() << "\n";
            } else {
                std::cout << "Order not found: " << orderId << "\n";
            }
        } else {
            std::cout << "Error: Unknown subcommand for 'list': " << subcommand << "\n";
        }
    }

    void printHelp() const {
        std::cout << "Available commands:\n";
        std::cout << "  help                  - Display this help message\n";
        std::cout << "  status                - Show the system status\n";
        std::cout << "  disable loading       - Disable loading activities\n";
        std::cout << "  enable loading        - Enable loading activities\n";
        std::cout << "  list products         - List all stored products\n";
        std::cout << "  list product [id]     - Display details for a specific product\n";
        std::cout << "  list orders           - List all stored orders\n";
        std::cout << "  list order [id]       - Display details for a specific order\n";
        std::cout << "  clear                 - Clear the terminal screen\n";
        std::cout << "  exit                  - Exit the shell\n";
    }

    void clearScreen() const {
        std::cout << "\33[2J\33[H";
    }

public:
    Shell() : m_stopShell(false), m_showLoadingBars(true), m_historyIndex(0) {
        m_outputMutex = std::make_shared<std::mutex>();
    }

    void run() {
        enableRawMode();
        char c;

        while (!m_stopShell.load()) {
            int terminalHeight = getTerminalHeight();
            {
                std::lock_guard<std::mutex> lock(*m_outputMutex);
                moveCursor(terminalHeight, 0);
                clearLine();
                std::cout << "Command > " << m_currentCommand;
                std::cout.flush();
            }

            c = getchar();

            if (c == '\n') {
                {
                    std::lock_guard<std::mutex> lock(*m_outputMutex);
                    m_commandHistory.push_back(m_currentCommand);
                    m_historyIndex = m_commandHistory.size();
                    moveCursor(terminalHeight, 0);
                    clearLine();
                    std::cout << "Command > " << m_currentCommand << std::endl;
                }

                handleCommand(m_currentCommand);
                m_currentCommand.clear();
            } else if (c == '\b' || c == 127) {
                if (!m_currentCommand.empty()) {
                    m_currentCommand.pop_back();
                }
            } else if (c == '\033') { // Arrow keys
                getchar();
                c = getchar();
                if (c == 'A' && m_historyIndex > 0) { // Up Arrow
                    m_historyIndex--;
                    m_currentCommand = m_commandHistory[m_historyIndex];
                } else if (c == 'B' && m_historyIndex < m_commandHistory.size()) { // Down Arrow
                    m_historyIndex++;
                    m_currentCommand = (m_historyIndex < m_commandHistory.size()) ? m_commandHistory[m_historyIndex] : "";
                }
            } else {
                m_currentCommand += c;
            }
        }

        disableRawMode();
    }

    std::shared_ptr<std::mutex> getOutputMutex() const {
        return m_outputMutex;
    }

    std::atomic<bool>& getShowLoadingBars() {
        return m_showLoadingBars;
    }

    std::atomic<bool>& getStopShell() {
        return m_stopShell;
    }
};
