#ifndef ACTION_HPP_
#define ACTION_HPP_

#include <iostream>
#include <string>

class Actionnable {
protected:
    std::string m_name;
    int m_processingTime;
public:
    explicit Actionnable(const std::string& name, int time) : m_name(name), m_processingTime(time) {}
    virtual ~Actionnable() = default;

    virtual void execute() = 0;
    int getProcessingTime() { return m_processingTime; }

    void displayProcessingTime() const {
        std::cout << "Processing time: " << m_processingTime << " ms" << std::endl;
    }

    const std::string getName() const {
        return m_name;
    }
};


#endif
