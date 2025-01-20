#ifndef ORDERGENERATOR_HPP
#define ORDERGENERATOR_HPP

#include "order.hpp"
#include "productdatabase.hpp"
#include "reference_manager.hpp"
#include <chrono>
#include <date/date.h>  // Ou <date/date.h> selon l'emplacement de la lib date

// Fonction utilitaire pour récupérer l'heure système arrondie à la seconde
inline date::sys_seconds nowSeconds() {
    using namespace std::chrono;
    return date::floor<seconds>(system_clock::now());
}

inline Order generateOrder(
    const std::string& reference,
    int numberOfProducts,
    const Author& author,
    const Address& deliveryAddress)
{
    // Récupère l'instance singleton de la base de données
    auto& db = ProductDatabase::getInstance();

    // Tente de créer un (ou plusieurs) produit(s) pour la référence demandée
    // createProduct retourne un std::vector<Product>
    auto newProducts = db.createProduct(reference, numberOfProducts);
    if (newProducts.empty()) {
        throw std::runtime_error(
            "generateOrder: Failed to create product(s) for reference '" + reference + "'.");
    }

    // On utilise le premier produit créé pour l'Order 
    // (si plusieurs produits sont créés, on pourrait générer plusieurs orders)
    const auto& product = newProducts.front();

    // Détermine la date/heure de commande (maintenant, arrondie à la seconde)
    date::sys_seconds orderTime = nowSeconds();

    // Ex. : deliveryTime = orderTime + 7 jours (personnalisez à volonté)
    date::sys_seconds deliveryTime = orderTime + std::chrono::hours(24 * 7);

    // Crée l'Order
    Order order(
        product.getProductReference(),
        orderTime,
        deliveryTime,
        author,
        deliveryAddress
    );

    return order;
}

#endif // ORDERGENERATOR_HPP
