#ifndef PARAMETERS_HPP_
#define PARAMETERS_HPP_

struct Time {
    int days;
    int hours;
    int minutes;
    int seconds;
    Time() : days(0), hours(0), minutes(1), seconds(0) {}
};

struct Parameters {
    float timescale;
    Time time;
    Parameters() : timescale(1.0f), time() {}
};

#endif
