#ifndef PARAMETERS_PARSER_HPP_
#define PARAMETERS_PARSER_HPP_


#include "parameters.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>


class ParametersParser {
public:
    static Parameters parseParameters(const std::string& filename) {
        Parameters config;
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

            if (jsonData.contains("time")) {
                Time time;
                auto jsonTime = jsonData["time"];
                if (jsonTime.contains("days") && jsonTime.contains("hours")
                    && jsonTime.contains("minutes") && jsonTime.contains("seconds")) {
                    time.days = jsonTime["days"].get<int>();
                    time.hours = jsonTime["hours"].get<int>();
                    time.minutes = jsonTime["minutes"].get<int>();
                    time.seconds = jsonTime["seconds"].get<int>();
                    config.time = time;
                } else {
                    std::cerr << "Parameters Parser Error: Time doesn\'t have the correct format.\n";
                }
            }
            return config;

        } catch (const std::exception& e) {
            std::cerr << "Error parsing config file: " << e.what() << std::endl;
        }

        return config;
    }
};


#endif
