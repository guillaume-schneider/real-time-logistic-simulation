#ifndef PRODUCTREFERENCE_HPP_
#define PRODUCTREFERENCE_HPP_

#include <string>
#include <iostream>

class ProductReference {
private:
    std::string m_name;
    std::string m_category;
    std::string m_subCategory;
    std::string m_year;
    std::string m_reference;

public:
    ProductReference() = default;

    ProductReference(const std::string& name, const std::string& category,
                     const std::string& subCategory, const std::string& year,
                     const std::string& reference)
        : m_name(name), m_category(category), m_subCategory(subCategory),
          m_year(year), m_reference(reference) {}

    const std::string& getName() const { return m_name; }
    const std::string& getCategory() const { return m_category; }
    const std::string& getSubCategory() const { return m_subCategory; }
    const std::string& getYear() const { return m_year; }
    const std::string& getReference() const { return m_reference; }

    void setName(const std::string& name) { m_name = name; }
    void setCategory(const std::string& category) { m_category = category; }
    void setSubCategory(const std::string& subCategory) { m_subCategory = subCategory; }
    void setYear(const std::string& year) { m_year = year; }
    void setReference(const std::string& reference) { m_reference = reference; }

    void display() const {
        std::cout << "Product Reference Details:\n"
                  << "Name: " << m_name << "\n"
                  << "Category: " << m_category << "\n"
                  << "Sub-Category: " << m_subCategory << "\n"
                  << "Year: " << m_year << "\n"
                  << "Reference: " << m_reference << "\n";
    }

    bool operator==(const ProductReference& other) const {
        return m_name == other.m_name &&
               m_category == other.m_category &&
               m_subCategory == other.m_subCategory &&
               m_year == other.m_year &&
               m_reference == other.m_reference;
    }

    bool operator!=(const ProductReference& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const ProductReference& product) {
        os << "Product Reference: [Name: " << product.m_name
           << ", Category: " << product.m_category
           << ", Sub-Category: " << product.m_subCategory
           << ", Year: " << product.m_year
           << ", Reference: " << product.m_reference << "]";
        return os;
    }

    static std::string toString(const ProductReference& product) {
        return product.m_category + product.m_subCategory + product.m_year.substr(2) +
            product.m_reference;
    }

    static ProductReference fromString(const std::string& reference) {
        if (reference.size() < 10) {
            throw std::invalid_argument("Invalid reference format: " + reference);
        }

        std::string category = reference.substr(0, 2);        // First 2 characters (e.g., "EL")
        std::string subCategory = reference.substr(2, 2);     // Next 2 characters (e.g., "PH")
        std::string year = "20" + reference.substr(4, 2);     // Next 2 characters prefixed by "20" (e.g., "24")
        std::string counter = reference.substr(6);           // Remaining characters (e.g., "0002")

        return ProductReference("", category, subCategory, year, reference);
    }
};

#endif // PRODUCTREFERENCE_HPP_
