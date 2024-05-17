#ifndef COMPUTER_CLUB_TABLE_HPP
#define COMPUTER_CLUB_TABLE_HPP

#include <string>

struct Table {
    static int hourlyRate;
    int number;
    int totalMinutesOccupied;
    int totalRevenue;

    std::string clientName;
    std::string startTime;

    bool isOccupied;

    int countRevenue(int minutesOccupied) noexcept;
    int countMinutesOccupied(std::string &startTime, std::string &endTime) noexcept;
};

#endif  // COMPUTER_CLUB_TABLE_HPP
