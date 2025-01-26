#ifndef INITIALIZER_HPP_
#define INITIALIZER_HPP_


#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "stock/product/product_database.hpp"
#include "stock/order/order_database.hpp"
#include "stock/reference/reference_manager.hpp"
#include "parameters.hpp"
#include "logistics/scheduler.hpp"
#include "logistics/site.hpp"


class Initializer {
private:
    std::string m_referencesPath;
    std::string m_productsPath;
    std::string m_ordersPath;
    std::string m_parametersPath;
    std::string m_logisticsPath;

    Initializer() = default;

    bool doFileContains(nlohmann::json config, const std::string& key);
    void parseJsonFile(const std::string& configFilePath);
public:
    static Initializer& Initializer::getInstance() {
        static Initializer instance;
        return instance;
    }

    Initializer(const Initializer&) = delete;
    Initializer& operator=(const Initializer&) = delete;
    bool injectArguments(int argc, char* argv[]);
    void loadData(ReferenceManager& refManager,
        OrderDatabase& orderDb, Parameters& parameters, Scheduler& ordonator, Site& site);
};


#endif
