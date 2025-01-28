#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>

int getTerminalHeight();
void moveCursorToCommandLine();
void clear();
void moveCursor(int row, int col);
void clearLine();
void disableRawMode();
void enableRawMode();
std::string trim(const std::string& str);
void clearScreen();

#endif
