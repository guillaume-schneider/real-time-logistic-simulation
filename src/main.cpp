#include "logistics/reference.hpp"
#include <iostream>


int main (void) {

    // Create a ProductReferenceManager
    ReferenceManager manager;

    // Add some products
    manager.addProduct("Smartphone X", "EL", "PH", "24");
    manager.addProduct("Smartphone Z", "EL", "PH", "24");
    manager.addProduct("Laptop Y", "EL", "LA", "24");
    manager.addProduct("Tablet Z", "EL", "TB", "24");

    std::cout << "display 1st manager: " << std::endl;
    manager.displayProducts();

    // Save products to JSON file
    manager.saveToJson("products.json");

    // Clear current product list
    ReferenceManager newManager;

    // Load products from JSON file
    newManager.loadFromJson("../products.json");

    // Display products
    newManager.displayProducts();

    return 0;
}
