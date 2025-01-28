#include "utils.hpp"
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <iostream>

int getTerminalHeight() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
}

void moveCursorToCommandLine() {
    int terminalHeight = getTerminalHeight();
    std::cout << "\33[" << terminalHeight << ";1H";
    std::cout << "Command > ";
    std::cout.flush();
}

void clear() {
    std::cout << "\33[2J";
}

void moveCursor(int row, int col) {
    std::cout << "\033[" << row << ";" << col << "H";
}

void clearLine() {
    std::cout << "\033[2K";
    std::cout.flush();
}

void enableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void disableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

void clearScreen() {
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}
