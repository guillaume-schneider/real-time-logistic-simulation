#ifndef ACTIONS_HPP_
#define ACTIONS_HPP_

#include "action.hpp"
#include "worker.hpp"
#include "coordinates.hpp"
#include <memory>

class Move : public Actionnable {
private:
    std::shared_ptr<Worker> m_movable;
    Point2D m_destination;
public:
    explicit Move(const std::string& name, int time,
        std::shared_ptr<Worker> movable, const Point2D& destination)
         : Actionnable(name, time), m_movable(movable), m_destination(destination) {}

    void execute() override {
        m_movable->setCoordinates(m_destination);
    }
};

#endif
