#include "ComputerClubTable.hpp"
#include "ComputerClubValidator.hpp"

int Table::hourlyRate = 0;

int Table::countMinutesOccupied(std::string &startTime, std::string &endTime) noexcept
{
    return Validator::convertTimeToMinutes(endTime) - Validator::convertTimeToMinutes(startTime);
}

int Table::countRevenue(int minutesOccupied) noexcept { return ceil(minutesOccupied / 60.0) * hourlyRate; }