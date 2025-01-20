#ifndef ORDER_HPP_
#define ORDER_HPP_

#include "product_reference.hpp"
#include "../logistics/location.hpp"
#include <string>
#include "date/date.h"

struct Author {
    std::string name;     // Author's name
    std::string contact;  // Contact information (optional, e.g., email or phone)

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
    std::string reference;      // Référence du produit
    std::string serialNumber;
    date::sys_seconds order_time;    // Timestamp de commande
    date::sys_seconds delivery_time; // Timestamp de livraison
    Author author;                                       // Auteur de la commande
    Address delivery_address;                            // Adresse de livraison

    Order() = default;
    Order(const std::string& ref, 
            const date::sys_seconds& orderTimestamp,
            const date::sys_seconds& deliveryTimestamp,
            const Author& orderAuthor, 
            const Address& deliveryAddr)
        : reference(ref),
            order_time(orderTimestamp),
            delivery_time(deliveryTimestamp),
            author(orderAuthor),
            delivery_address(deliveryAddr) {}

    std::string toString() const {
            std::ostringstream oss;
            oss << "Order Information:\n"
                << "Reference: " << reference << "\n"
                << "Order Timestamp: " << date::format("%F %T", order_time) << "\n"
                << "Delivery Timestamp: " << date::format("%F %T", delivery_time) << "\n"
                << "Author: " << author.toString() << "\n"
                << "Delivery Address: " << delivery_address.toString();
            return oss.str();
    }
};

#endif
