#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <queue>
#include <regex>
#include <iterator>
#include <sstream>
#include <list>
#include "Exceptions.hpp"

class Client {
public:
    std::string name;
    int tableNumber;
    std::string arrivalTime;
    std::string departureTime;
};

class Table {
public:
    static int hourlyRate; 

    int number;
    bool isOccupied;
    int totalMinutesOccupied;
    int totalRevenue;
    std::string clientName;
    std::string startTime;

    int countRevenue(int minutesOccupied);
    int countMinutesOccupied(std::string &startTime, std::string &endTime);
};

class Event {
public:
    std::string time;
    int type;
    std::string body;
    int tableNumber = -1;
};

class ComputerClub {
    int tablesNumber;
    int freeTables;
    std::string openingTime;
    std::string closingTime;
    int hourlyRate;

    std::vector<Table> tables;
    std::list<Event> events;
    std::queue<std::string> waitingQueue;
    std::map<std::string, int> clientTableMap;

    using EventFunction = void (ComputerClub::*)(Event&);
    std::map<int, EventFunction> eventHandlers;

public:
    ComputerClub();

    Event newEvent;
    bool newEventGenerated = false;

    void readInput(const std::string &filename, std::vector<std::string>& inputLines);
    void readInitialParameters(std::ifstream& file);
    void readEvents(std::vector<std::string>& inputLines);
    void processEvents();
    void setNewEvent(std::string time, int type, std::string body, int tableNumber);
    void writeOutput();
    bool checkAndHandleError(bool condition, Event &event, const std::string &errorMessage);

    void inputClientArrives(Event &event);
    void inputClientSits(Event &event);
    void inputClientWaits(Event &event);
    void inputClientLeaves(Event &event);
    void outputClientLeaves(Event &event);
    void outputClientSits(Event &event);
    void outputErrorEvent(Event &event);

    void freeTable(int tableNumber, std::string &time);

    void endOfDayEvents();
};

int convertTimeToMinutes(const std::string &time) {
        int hours = std::stoi(time.substr(0, 2));
        int minutes = std::stoi(time.substr(3, 2));
        return hours * 60 + minutes;
}

// COMPUTER CLUB IMPLEMENTATION

ComputerClub::ComputerClub() {
    // input event types
    eventHandlers[1] = &ComputerClub::inputClientArrives;
    eventHandlers[2] = &ComputerClub::inputClientSits;
    eventHandlers[3] = &ComputerClub::inputClientWaits;
    eventHandlers[4] = &ComputerClub::inputClientLeaves;
    // output event types
    eventHandlers[11] = &ComputerClub::outputClientLeaves;
    eventHandlers[12] = &ComputerClub::outputClientSits;
    eventHandlers[13] = &ComputerClub::outputErrorEvent;
}

// CHECKING THE INPUT FORMAT FUNCTIONS

bool isValidTimeFormat(const std::string& time) {
    std::regex timeFormat("^([01][0-9]|2[0-3]):[0-5][0-9]$");
    return std::regex_match(time, timeFormat);
}

bool isValidClientName(const std::string& name) {
    std::regex nameFormat("^[a-z0-9_-]+$");
    return std::regex_match(name, nameFormat);
}

bool isValidTableNumber(const std::string& stringNumber, int maxNumber) {
    try {
        int number = std::stoi(stringNumber);
        return 1 <= number && number <= maxNumber;
    } catch (std::invalid_argument& e) {
        return false;
    }
}

bool isValidEventId(const std::string& id) {
    try {
        int number = std::stoi(id);
        return 1 <= number && number <= 4 || 11 <= number && number <= 13;
    } catch (std::invalid_argument& e) {
        return false;
    }
}

bool isValidLine(const std::string &line, int maxTableNumber, std::string &previousEventTime) {
    std::istringstream iss(line);
        std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>());

        if (tokens.size() < 3) {
            return false;
        }

        if (!isValidTimeFormat(tokens[0]) || !isValidEventId(tokens[1])) {
            return false;
        }
        if (!previousEventTime.empty() && convertTimeToMinutes(tokens[0]) < convertTimeToMinutes(previousEventTime)) {
            return false;
        }

        previousEventTime = tokens[0];
        if (std::stoi(tokens[1]) == 2) {
            if (tokens.size() != 4 || !isValidClientName(tokens[2]) || !isValidTableNumber(tokens[3], maxTableNumber)) {
                return false;
            }
        } else {
            if (!isValidClientName(tokens[2])) {
                return false;
            }
        }

    return true;
}

// READING INPUT FUNCTIONS

void ComputerClub::readInitialParameters(std::ifstream& file) {
    file >> tablesNumber >> openingTime >> closingTime >> Table::hourlyRate;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    tables.resize(tablesNumber + 1);
    for (int id = 1; id <= tablesNumber; ++id) {
        tables[id].number = id;
    }
    freeTables = tablesNumber;
}

void ComputerClub::readEvents(std::vector<std::string>& inputLines) {
    Event event;
    for (const auto& line : inputLines) {
        std::istringstream iss(line);
        std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>());
        event.time = tokens[0];
        event.type = std::stoi(tokens[1]);
        event.body = tokens[2];
        event.tableNumber = tokens.size() == 4 ? std::stoi(tokens[3]) : -1;
        events.push_back(event);
    }
}

void ComputerClub::readInput(const std::string &filename, std::vector<std::string>& inputLines) {
    std::ifstream file(filename);
    if (!file) {
        throw FileOpenError(filename);
    }

    readInitialParameters(file);

    std::string line;
    std::string previousEventTime = "";
    while (std::getline(file, line)) {
        if (!isValidLine(line, tablesNumber, previousEventTime)) {
            throw InputFormatError(line);
        }
        inputLines.push_back(line);
    }

    readEvents(inputLines);

    file.close();
}

bool ComputerClub::checkAndHandleError(bool condition, Event &event, const std::string &errorMessage) {
    if (!condition) {
        Event errorEvent{event.time, 13, errorMessage};
        (this->*eventHandlers[13])(errorEvent);
        return true;
    }
    return false;
}

// INPUT EVENTS

void ComputerClub::inputClientArrives(Event &event) {
    if (checkAndHandleError(clientTableMap.find(event.body) == clientTableMap.end(), event, "YouShallNotPass")) return;
    if (checkAndHandleError(convertTimeToMinutes(openingTime) <= convertTimeToMinutes(event.time), event, "NotOpenYet")) return;

    clientTableMap[event.body] = -1; // -1 means the client is not at any table
}

void ComputerClub::inputClientSits(Event &event) {
    if (checkAndHandleError(!tables[event.tableNumber].isOccupied, event, "PlaceIsBusy")) return;
    if (checkAndHandleError(clientTableMap.find(event.body) != clientTableMap.end(), event, "ClientUnknown")) return;

    if (clientTableMap[event.body] != -1) {
        freeTable(clientTableMap[event.body], event.time);
    }
    tables[event.tableNumber].isOccupied = true;
    tables[event.tableNumber].clientName = event.body;
    tables[event.tableNumber].startTime = event.time;
    clientTableMap[event.body] = event.tableNumber;
    freeTables--;
}

void ComputerClub::inputClientWaits(Event &event) {
    if (checkAndHandleError(freeTables <= 0, event, "ICanWaitNoLonger!")) return;

    if (tablesNumber < waitingQueue.size()) {
        (this->*eventHandlers[11])(event);
        return;
    }

    waitingQueue.push(event.body);
}

void ComputerClub::inputClientLeaves(Event &event) {
    std::string clientName = event.body;
    if (checkAndHandleError(clientTableMap.find(clientName) != clientTableMap.end(), event, "ClientUnknown")) return;

    int tableNumber = clientTableMap[event.body];
    clientTableMap.erase(event.body);
    if (tableNumber == -1) {
        return;
    }
    freeTable(tableNumber, event.time);

    if (!waitingQueue.empty()) {
        std::string nextClient = waitingQueue.front();
        waitingQueue.pop();
        Event nextEvent{event.time, event.type, nextClient, tableNumber};
        (this->*eventHandlers[12])(nextEvent);
    }
}

// OUTPUT EVENTS

void ComputerClub::setNewEvent(std::string time, int type, std::string body, int tableNumber = -1) {
    newEvent.time = time;
    newEvent.type = type;
    newEvent.body = body;
    newEvent.tableNumber = tableNumber;
}

void ComputerClub::outputClientLeaves(Event &event) {
    newEventGenerated = true;
    setNewEvent(event.time, 11, event.body);
    int tableNumber = clientTableMap[event.body];
    if (tableNumber != -1) {
        freeTable(tableNumber, event.time);
    }
    clientTableMap.erase(event.body);
}

void ComputerClub::outputClientSits(Event &event) {
    newEventGenerated = true;
    setNewEvent(event.time, 12, event.body, event.tableNumber);
    tables[event.tableNumber].isOccupied = true;
    tables[event.tableNumber].clientName = event.body;
    tables[event.tableNumber].startTime = event.time;
    tables[event.tableNumber].number = event.tableNumber;
    clientTableMap[event.body] = event.tableNumber;
}

void ComputerClub::outputErrorEvent(Event &event) {
    newEventGenerated = true;
    setNewEvent(event.time, 13, event.body);
}

void ComputerClub::processEvents() {
    for (auto it = events.begin(); it != events.end(); ++it) {
            (this->*eventHandlers[it->type])(*it);
            if (newEventGenerated) {
                events.insert(std::next(it), newEvent);
                newEventGenerated = false;
                ++it;
            }
    }
}
void ComputerClub::writeOutput() {
    std::cout << openingTime << std::endl;
    
    for (const auto &event : events) {
        std::cout << event.time << " " << event.type << " " << event.body << " " << (event.tableNumber == -1 ? "" : std::to_string(event.tableNumber)) << std::endl;
    }
    std::cout << closingTime << std::endl;

    for (int i = 1; i <= tablesNumber; ++i) {
        auto &table = tables[i];
        std::cout << table.number << " " << table.totalRevenue << " ";
        // Output in the format HH:MM
        std::cout << std::setw(2) << std::setfill('0') << table.totalMinutesOccupied / 60 << ":";
        std::cout << std::setw(2) << std::setfill('0') << table.totalMinutesOccupied % 60 << std::endl;
    }
}

void ComputerClub::freeTable(int tableNumber, std::string &time) {
    Table &table = tables[tableNumber];
    table.isOccupied = false;
    int minutesOccupied = table.countMinutesOccupied(table.startTime, time);
    table.totalMinutesOccupied += minutesOccupied;
    table.totalRevenue += table.countRevenue(minutesOccupied);
    freeTables++;
}

void ComputerClub::endOfDayEvents() {
    while (!clientTableMap.empty()) {
        auto client = clientTableMap.begin();
        Event event{closingTime, 11, client->first};
        events.push_back(event);
        (this->*eventHandlers[event.type])(event);
    }
}

// TABLE CLASS IMPLEMENTATION

int Table::hourlyRate = 0;

int Table::countMinutesOccupied(std::string &startTime, std::string &endTime) {
    return convertTimeToMinutes(endTime) - convertTimeToMinutes(startTime);
}

int Table::countRevenue(int minutesOccupied) {
    return ceil(minutesOccupied / 60.0) * hourlyRate;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        throw InvalidNumberOfArgumentsError("1");
    }

    ComputerClub club;
    std::vector<std::string> inputLines;

    club.readInput(argv[1], inputLines);
    club.processEvents();
    club.endOfDayEvents();
    club.writeOutput();

    return 0;
}