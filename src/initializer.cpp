#include "initializer.hpp"
#include "parameters_parser.hpp"
#include "logistics/ordonator.hpp"


bool Initializer::doFileContains(nlohmann::json config, const std::string& key) {
    if (!config.contains(key)) {
        std::runtime_error("Error: Config file doesn\'t contains " + key + ".\n");
        return false;
    }
    return true;
}


void Initializer::parseJsonFile(const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);

    if (!configFile.is_open()) {
        throw std::runtime_error("Error: Can\'t open json file: " + configFilePath);
    }

    nlohmann::json config;
    configFile >> config;

    if (!doFileContains(config, "references") || !doFileContains(config, "products")
        || !doFileContains(config, "orders") || !doFileContains(config, "parameters")
        || !doFileContains(config, "logistics"))
        return;

    m_referencesPath = config["references"];
    m_productsPath = config["products"];
    m_ordersPath = config["orders"];
    m_parametersPath = config["parameters"];
    m_logisticsPath = config["logistics"];
}

bool Initializer::injectArguments(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage : " << argv[0] << " --config <file_path_json>\n";
        return false;
    }

    std::string configFilePath;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--config" && i + 1 < argc) {
            configFilePath = argv[i + 1];
            break;
        }
    }

    if (configFilePath.empty()) {
        std::cerr << "Error : You must give a configuration file with --config <file_path_json>\n";
        return false;
    }
    parseJsonFile(configFilePath);

    return true;
}


void Initializer::loadData(ReferenceManager& refManager, ProductDatabase& productDb,
    OrderDatabase& orderDb, Parameters& parameters, Ordonator& ordonator) {
    refManager.loadFromJson(m_referencesPath);
    productDb.init(m_productsPath, refManager);
    orderDb.loadFromFile(m_ordersPath);
    parameters = ParametersParser::parseParameters(m_parametersPath);
    ordonator.setParameters(&parameters);
    ordonator.loadFromFile(m_logisticsPath);
}
