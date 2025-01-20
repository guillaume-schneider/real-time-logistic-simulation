#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

struct Time {
    int days;
    int hours;
    int minutes;
    int seconds;
    Time() : days(0), hours(0), minutes(1), seconds(0) {}
};

struct Config {
    float timescale;
    Time time;
    Config() : timescale(1.0f), time() {}
};

class ConfigParser {
public:
    static Config parseConfig(const std::string& filename) {
        Config config;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open config file: " << filename << std::endl;
            return config;
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

            if (jsonData.contains("time") && jsonData["time"].is_string()) {
                Time time;
                time.days = jsonData["days"].get<int>();
                time.hours = jsonData["hours"].get<int>();
                time.minutes = jsonData["minutes"].get<int>();
                time.seconds = jsonData["seconds"].get<int>();
                config.time = time;
            }
            return config;

        } catch (const std::exception& e) {
            std::cerr << "Error parsing config file: " << e.what() << std::endl;
        }

        return config;
    }
};

#endif
