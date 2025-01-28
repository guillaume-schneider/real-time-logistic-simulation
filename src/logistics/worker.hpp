#ifndef WORKER_HPP_
#define WORKER_HPP_

#include "actionner.hpp"
#include "tool_type.hpp"
#include "task.hpp"
#include "../stock/order/order.hpp"
#include "../stock/product/product.hpp"
#include <memory>

class Worker : public Actionner {
private:
    ToolType m_currentTool;
    float m_speed;
    Order* m_currentOrder;
    std::unique_ptr<Product> m_currentProduct;
    Point2D m_currentCoodinates;
public:
    Worker() : Actionner(), m_speed(5.0f), m_currentTool(None), m_currentCoodinates(Point2D()), m_currentOrder(nullptr) {}
    Worker(int actionnerId, const std::string& name, std::shared_ptr<std::mutex> outputMtx,
            std::atomic<bool>& isEnteringCommand, Parameters* config = nullptr, const Point2D& coordinates = Point2D(),
            const int maxTaskSize = 100, float speed = 8.0f)
        : Actionner(actionnerId, name, outputMtx, isEnteringCommand, config, maxTaskSize),
            m_speed(speed), m_currentTool(None), m_currentCoodinates(coordinates), m_currentOrder(nullptr) {}

    void setTool(const ToolType& type) {
        m_currentTool = type;
    }

    const ToolType& getTool() const {
        return m_currentTool;
    }

    const int& getId() const {
        return m_id;
    }

    void setSpeed(const float& speed) {
        m_speed = speed;
    }

    const float& getSpeed() const {
        return m_speed;
    }

    void setOrder(Order& order) {
        m_currentOrder = &order;
    }

    Order* getOrder() const {
        return m_currentOrder;
    }

    void takeProduct(std::unique_ptr<Product> product) {
        m_currentProduct = std::move(product);
    }

    std::unique_ptr<Product> releaseProduct() {
        if (!m_currentProduct) {
            // No product to deposit
            std::cerr << "Worker is not holding any product to deposit.\n";
            return nullptr;
        }

        auto resultProduct = std::move(m_currentProduct);
        m_currentProduct.reset();
        return resultProduct;
    }

    void setCoordinates(const Point2D& coordinates) { m_currentCoodinates = coordinates; }
    void setCoordinates(const double& x, const double& y) { 
        m_currentCoodinates.x = x;
        m_currentCoodinates.y = y;
    }
    const Point2D& getCoordinates() const { return m_currentCoodinates; }

    std::string getName() const {
        return m_name;
    }
};
#endif
