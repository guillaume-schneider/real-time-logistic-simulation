#ifndef ORDER_FACTORY_HPP_
#define ORDER_FACTORY_HPP_
#include <sstream>
#include <atomic>
#include <iomanip>
#include "order.hpp"

class OrderFactory {
private:
    static std::atomic<int> counter;

    static std::string generateOrderReference() {
        std::ostringstream oss;

        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm result;

#ifdef _WIN32
        gmtime_s(&result, &now_time_t); // Windows
#else
        gmtime_r(&now_time_t, &result); // Linux/Unix
#endif

        oss << "ORD-"
            << std::put_time(&result, "%Y%m%d-%H%M%S") << "-"
            << std::setw(4) << std::setfill('0') << counter++;

        return oss.str();
    }

public:
    static Order createOrder(
        const std::string& productReference,
        const date::sys_seconds& orderTime,
        const Author& author,
        const Address& deliveryAddress
    ) {
        std::string orderReference = generateOrderReference();
        return Order(orderReference, productReference, orderTime, author, deliveryAddress);
    }
};

#endif
