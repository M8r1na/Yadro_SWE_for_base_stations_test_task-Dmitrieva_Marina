#ifndef COMPUTER_CLUB_VALIDATOR_HPP
#define COMPUTER_CLUB_VALIDATOR_HPP

#include <string>

struct Validator {
    static int convertTimeToMinutes(const std::string &time) noexcept;

    static bool isValidTimeFormat(const std::string &time) noexcept;
    static bool isValidClientName(const std::string &name) noexcept;
    static bool isValidTableNumber(const std::string &stringNumber, int maxNumber);
    static bool isValidEventId(const std::string &id);
    static bool isValidLine(const std::string &line, int maxTableNumber, std::string &previousEventTime);
};

#endif  // COMPUTER_CLUB_VALIDATOR_HPP