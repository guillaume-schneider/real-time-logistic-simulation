#ifndef TOOLS_HPP_
#define TOOLS_HPP_


#include "actionner.hpp"
#include "order.hpp"
#include "product.hpp"


class Forklift : public Actionner {
private:
    Order m_currentOrder;
    Product m_currentProduct;
public:
    Forklift() = default;
    
};

class Conveyor : public Actionner {
    Conveyor() = default;
};

#endif
