#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>

struct Event {
    Event() noexcept = default;
    Event(std::string time, int type, std::string body, int tableNumber = -1) noexcept
        : time(time), type(type), body(body), tableNumber(tableNumber) {}

    std::string time;
    std::string body;
    int type;
    int tableNumber = -1;
};

#endif  // EVENT_HPP