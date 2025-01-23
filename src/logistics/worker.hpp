#ifndef WORKER_HPP_
#define WORKER_HPP_

#include "actionner.hpp"
#include "tool_type.hpp"
#include "task.hpp"

class Worker : public Actionner {
private:
    ToolType m_currentTool;
public:
    Worker() : Actionner() {}
    Worker(int actionnerId, const std::string& name, std::mutex* outputMtx,
            Parameters* config = nullptr, const Point2D& coordinates = Point2D(),
            const int maxTaskSize = 100)
        : Actionner(actionnerId, name, outputMtx, config, coordinates, maxTaskSize) {}

    void setTool(const ToolType& type) {
        m_currentTool = type;
    }

    const ToolType& getTool() const {
        return m_currentTool;
    }

    const int& getId() const {
        return m_id;
    }
};
#endif
