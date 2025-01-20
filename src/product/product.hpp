#ifndef PRODUCT_HPP_
#define PRODUCT_HPP_

#include <string>
#include "product_reference.hpp"
#include "../logistics/coordinates.hpp"

class Product {
private:
    std::string m_serialNumber;
    std::string m_reference;
    Point2D m_coordinates;
public:
    Product() = default;
    Product(std::string serialNumber, std::string reference,
        Point2D coordinates) 
        : m_serialNumber(serialNumber), m_reference(reference),
            m_coordinates(coordinates) {}
    Product(const Product& other)
        : m_serialNumber(other.m_serialNumber), m_reference(other.m_reference),
            m_coordinates(other.m_coordinates) {}
    Product& operator=(const Product& other) {
        if (&other == this) return *this;
        m_serialNumber = other.m_serialNumber;
        m_reference = other.m_reference;
        m_coordinates = other.m_coordinates;
        return *this;
    }
    ~Product() = default;

    std::string getSerialNumber() const { return m_serialNumber; }
    std::string getProductReference() const { return m_reference; }
    void setCoordinates(const Point2D coordinates) { m_coordinates = coordinates; }
    Point2D getCoordinates() const { return m_coordinates; }
};

#endif
