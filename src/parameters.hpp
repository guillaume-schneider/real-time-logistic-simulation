#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "config.hpp"

struct Config {
    float timescale = 1.0f;
};

class ConfigParser {
public:
    static void parseConfig(const std::string& filename, Config& config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open config file: " << filename << std::endl;
            return;
        }

        try {
            nlohmann::json jsonData;
            file >> jsonData;

            // Parse and validate timescale
            if (jsonData.contains("timescale") && jsonData["timescale"].is_number()) {
                config.timescale = jsonData["timescale"].get<float>();
                if (config.timescale < 0.0f) {
                    std::cerr << "Warning: Timescale cannot be negative. Resetting to default (1.0)." << std::endl;
                    config.timescale = 1.0f;
                }
            }

            // // Parse and validate maxThreads
            // if (jsonData.contains("maxThreads") && jsonData["maxThreads"].is_number_integer()) {
            //     config.maxThreads = jsonData["maxThreads"].get<int>();
            //     if (config.maxThreads <= 0) {
            //         std::cerr << "Warning: maxThreads must be greater than 0. Resetting to default (4)." << std::endl;
            //         config.maxThreads = 4;
            //     }
            // }

            // // Parse debugMode
            // if (jsonData.contains("debugMode") && jsonData["debugMode"].is_boolean()) {
            //     config.debugMode = jsonData["debugMode"].get<bool>();
            // }

        } catch (const std::exception& e) {
            std::cerr << "Error parsing config file: " << e.what() << std::endl;
        }
    }
};

#endif
