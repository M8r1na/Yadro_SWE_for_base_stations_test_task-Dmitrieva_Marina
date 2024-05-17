#include "ComputerClubValidator.hpp"
#include <iterator>
#include <regex>
#include <sstream>
#include <vector>

const int MIN_EVENT_ID = 1;
const int MAX_EVENT_ID = 4;
const int SITS_EVENT_ID = 2;
const int MIN_TABLE_NUMBER = 1;
const int TOKENS_IN_SITS_EVENT = 4;
const int TOKENS_IN_OTHER_EVENT = 3;

int Validator::convertTimeToMinutes(const std::string &time) noexcept
{
    int hours = std::stoi(time.substr(0, 2));
    int minutes = std::stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}

bool Validator::isValidTimeFormat(const std::string &time) noexcept
{
    std::regex timeFormat("^([01][0-9]|2[0-3]):[0-5][0-9]$");
    return std::regex_match(time, timeFormat);
}

bool Validator::isValidClientName(const std::string &name) noexcept
{
    std::regex nameFormat("^[a-z0-9_-]+$");
    return std::regex_match(name, nameFormat);
}

bool Validator::isValidTableNumber(const std::string &stringNumber, int maxNumber)
{
    try {
        int number = std::stoi(stringNumber);
        return MIN_TABLE_NUMBER <= number && number <= maxNumber;
    } catch (std::invalid_argument &e) {
        return false;
    }
}

bool Validator::isValidEventId(const std::string &id)
{
    try {
        int number = std::stoi(id);
        return MIN_EVENT_ID <= number && number <= MAX_EVENT_ID;
    } catch (std::invalid_argument &e) {
        return false;
    }
}

bool Validator::isValidLine(const std::string &line, int maxTableNumber,
                            std::string &previousEventTime)
{
    std::istringstream iss(line);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>());

    if (tokens.size() < TOKENS_IN_OTHER_EVENT || TOKENS_IN_SITS_EVENT < tokens.size()) {
        return false;
    }

    std::string time = tokens[0];
    std::string eventId = tokens[1];
    std::string clientName = tokens[2];

    if (!isValidTimeFormat(time) || !isValidEventId(eventId) || !isValidClientName(clientName)) {
        return false;
    }
    if (!previousEventTime.empty()
        && convertTimeToMinutes(time) < convertTimeToMinutes(previousEventTime)) {
        return false;
    }

    previousEventTime = time;
    if (std::stoi(eventId) == SITS_EVENT_ID) {
        std::string sitsTableNumber = tokens[3];
        if (tokens.size() != TOKENS_IN_SITS_EVENT || !isValidTableNumber(sitsTableNumber, maxTableNumber)) {
            return false;
        }
    }

    return true;
}