#ifndef ORDER_HPP_
#define ORDER_HPP_

#include "../reference/product_reference.hpp"
#include "../../logistics/location.hpp"
#include <string>
#include "date/date.h"
#include <optional>

struct Author {
    std::string name;
    std::string contact;

    Author(const std::string& authorName, const std::string& authorContact = "")
        : name(authorName), contact(authorContact) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "Author: " << name;
        if (!contact.empty()) {
            oss << " (Contact: " << contact << ")";
        }
        return oss.str();
    }
};


struct Order {
public:
    std::string orderReference;
    std::string productReference;
    std::optional<std::string> serialNumber;
    date::sys_seconds orderTime;
    std::optional<date::sys_seconds> deliveryTime; // Timestamp de livraison
    Author author;                                       // Auteur de la commande
    Address deliveryAddress;                            // Adresse de livraison

    Order()
        : orderReference("UNKNOWN"),
          productReference("UNKNOWN"),
          orderTime(std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now())),
          author("Unknown Author"),
          deliveryAddress("Unknown Street", "Unknown City", "Unknown Country", "00000") {}
    Order(const std::string& orderReference,
          const std::string& productReference,
          const date::sys_seconds& orderTime,
          const Author& author,
          const Address& deliveryAddress)
        : orderReference(orderReference),
          productReference(productReference),
          orderTime(orderTime),
          author(author),
          deliveryAddress(deliveryAddress) {}

    std::string toString() const {
        std::ostringstream oss;
        oss << "Order Information:\n"
            << "Order Reference: " << orderReference << "\n"
            << "Product Reference: " << productReference << "\n"
            << "Order Timestamp: " << date::format("%F %T", orderTime) << "\n";

        if (deliveryTime) {
            oss << "Delivery Timestamp: " << date::format("%F %T", *deliveryTime) << "\n";
        } else {
            oss << "Delivery Timestamp: N/A\n";
        }

        if (serialNumber) {
            oss << "Serial Number: " << *serialNumber << "\n";
        } else {
            oss << "Serial Number: N/A\n";
        }

        oss << "Author: " << author.toString() << "\n"
            << "Delivery Address: " << deliveryAddress.toString();
        return oss.str();
    }
};

#endif
