#include <algorithm>
#include <vector>
#include "order_database.hpp"


bool OrderDatabase::doFileContains(const std::string& filename,
    const nlohmann::json_abi_v3_11_3::json& item, std::string key) {
    if (!item.contains(key)) {
        std::cerr << "\'" << filename << "\' doesn\'t contain " << "\'" << key << "\'\n";
        return false;
    }
    return true;
}


date::sys_seconds OrderDatabase::convertStringToTime(const std::string& time) {
    std::string timestamp = "2025-01-20T10:00:00Z";

    std::istringstream iss(timestamp);
    date::sys_seconds orderTime;
    iss >> date::parse("%FT%TZ", orderTime);

    if (iss.fail()) {
        std::cerr << "Failed to parse timestamp: " << timestamp << "\n";
        return orderTime;
    }
    return orderTime;
}


void OrderDatabase::addOrder(const Order& order) {
    m_orders[order.orderReference] = order;
}


bool OrderDatabase::removeOrder(const std::string& orderReference) {
    return m_orders.erase(orderReference) > 0;
}


Order& OrderDatabase::createOrder(
    const std::string& productReference,
    const date::sys_seconds& orderTime,
    const Author& author,
    const Address& deliveryAddress
) {
    Order order = OrderFactory::createOrder(productReference, orderTime,
                                            author, deliveryAddress);
    m_orders[order.orderReference] = order;
    return m_orders[order.orderReference];
}


std::optional<Order> OrderDatabase::getOrder(const std::string& orderReference) const {
    auto it = m_orders.find(orderReference);
    if (it != m_orders.end()) {
        return it->second;
    }
    return std::nullopt;
}


std::vector<Order> OrderDatabase::getOrdersByProductReference(const std::string& productReference) const {
    std::vector<Order> result;
    for (const auto& [ref, order] : m_orders) {
        if (order.productReference == productReference) {
            result.push_back(order);
        }
    }
    return result;
}


std::vector<Order> OrderDatabase::getOrders() const {
    std::vector<Order> allOrders;
    allOrders.reserve(m_orders.size());
    std::transform(m_orders.begin(), m_orders.end(), std::back_inserter(allOrders),
                   [](const auto& pair) { return pair.second; });
    return allOrders;
}


void OrderDatabase::loadFromFile(const std::string& filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        throw std::runtime_error("Impossible d'ouvrir le fichier " + filename);
    }

    json j;
    inputFile >> j;

    for (const auto& item : j) {
        if (item.is_object()) {
            Order order;
            if (!doFileContains(filename, item, "product_reference") 
                || !doFileContains(filename, item, "order_time")
                || !doFileContains(filename, item, "author")
                    || !doFileContains(filename, item["author"], "name")
                || !doFileContains(filename, item, "delivery_address")
                    || !doFileContains(filename, item["delivery_address"], "street")
                    || !doFileContains(filename, item["delivery_address"], "city")
                    || !doFileContains(filename, item["delivery_address"], "country")
                    || !doFileContains(filename, item["delivery_address"], "postal_code"))
                continue;

            order.productReference = item["product_reference"].get<std::string>();
            order.orderTime = convertStringToTime(item["order_time"].get<std::string>());
            auto name = item["author"]["name"].get<std::string>();
            std::string contact;
            if (item["author"].contains("contact")) {
                contact = item["author"]["contact"].get<std::string>();
            }
            order.author = Author(name, contact);
            auto street = item["delivery_address"]["street"];
            auto city = item["delivery_address"]["city"];
            auto country = item["delivery_address"]["country"];
            auto postalCode = item["delivery_address"]["postal_code"];
            addOrder(order);
        }
    }
}


void OrderDatabase::saveToFile(const std::string& filename) const {
    std::ofstream outputFile(filename);
    if (!outputFile) {
        throw std::runtime_error("Impossible d'écrire dans le fichier " + filename);
    }

    json j = json::array();
    for (const auto& [ref, order] : m_orders) {
        json item;

        item["order_reference"] = order.orderReference;
        item["product_reference"] = order.productReference;
        item["order_time"] = date::format("%FT%TZ", order.orderTime);

        if (order.serialNumber.has_value()) {
            item["serialNumber"] = *order.serialNumber;
        } else {
            item["serialNumber"] = nullptr;
        }

        if (order.deliveryTime.has_value()) {
            item["delivery_time"] = date::format("%FT%TZ", *order.deliveryTime);
        } else {
            item["delivery_time"] = nullptr;
        }

        item["author"] = {
            {"name", order.author.name},
            {"contact", order.author.contact}
        };

        item["delivery_address"] = {
            {"street", order.deliveryAddress.street},
            {"city", order.deliveryAddress.city},
            {"country", order.deliveryAddress.country},
            {"postal_code", order.deliveryAddress.zip}
        };
        j.push_back(item);
    }

    outputFile << j.dump(4);
}


void OrderDatabase::printAllOrders() const {
    for (const auto& [ref, order] : m_orders) {
        std::cout << order.toString() << "\n";
    }
}
