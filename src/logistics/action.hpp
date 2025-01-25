#ifndef ACTION_HPP_
#define ACTION_HPP_

#include <iostream>
#include <string>

class Actionnable {
protected:
    std::string m_name;
    long long m_processingTime;
public:
    explicit Actionnable(const std::string& name, long long time) : m_name(name), m_processingTime(time) {}
    virtual ~Actionnable() = default;

    virtual void execute() = 0;
    long long getProcessingTime() { return m_processingTime; }

    void displayProcessingTime() const {
        std::cout << "Processing time: " << m_processingTime << " ms" << std::endl;
    }

    const std::string getName() const {
        return m_name;
    }
};


#endif
