#ifndef ORDER_DATABASE_HPP_
#define ORDER_DATABASE_HPP_

#include "order.hpp"
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class OrderDatabase {
private:
    std::unordered_map<std::string, std::shared_ptr<Order>> m_orders;

    bool doFileContains(const std::string& filename,
        const nlohmann::json_abi_v3_11_3::json& item, std::string key);
    date::sys_seconds convertStringToTime(const std::string& time);
    OrderDatabase() = default; 
public:
    static OrderDatabase& getInstance() {
        static OrderDatabase instance;
        return instance;
    }
    OrderDatabase(const OrderDatabase&) = delete;
    OrderDatabase& operator=(const OrderDatabase&) = delete;
    void addOrder(const Order& order);
    bool removeOrder(const std::string& orderReference);
    std::shared_ptr<Order> createOrder(const std::string& productReference,
        const date::sys_seconds& orderTime, const Author& author, 
            const Address& deliveryAddress);
    std::shared_ptr<Order> getOrder(const std::string& orderReference) const;
    std::vector<std::shared_ptr<Order>> getOrders() const;
    std::vector<std::shared_ptr<Order>> getOrdersByProductReference(const std::string& productReference) const;
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void printAllOrders() const;
};

#endif
