#include "ComputerClub.hpp"
#include "Exceptions.hpp"
#include <fstream>
#include <iostream>

const int ARGUMENTS_NUMBER = 1;

void runClub(const std::string &filename)
{
    ComputerClub club;
    std::vector<std::string> inputLines;

    club.readInput(filename, inputLines);
    club.processEvents();
    club.endOfDayEvents();
    club.writeOutput();
}

int main(int argc, char *argv[])
{
    try {
        if (argc != ARGUMENTS_NUMBER + 1) {
            throw InvalidNumberOfArgumentsError(ARGUMENTS_NUMBER);
        }

        runClub(argv[1]);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}