#include "ComputerClub.hpp"
#include "ComputerClubValidator.hpp"
#include "Exceptions.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

const int TOKENS_IN_SITS_EVENT = 4;

// FUNCTION DEFINITIONS

enum EventType {
    INPUT_CLIENT_ARRIVES = 1,
    INPUT_CLIENT_SITS = 2,
    INPUT_CLIENT_WAITS = 3,
    INPUT_CLIENT_LEAVES = 4,
    OUTPUT_CLIENT_LEAVES = 11,
    OUTPUT_CLIENT_SITS = 12,
    OUTPUT_ERROR_EVENT = 13
};

// GENERATED EVENT

void ComputerClub::setNewEvent(std::string time, int type, std::string body,
                               int tableNumber = NOT_AT_TABLE)
{
    newEvent.time = time;
    newEvent.type = type;
    newEvent.body = body;
    newEvent.tableNumber = tableNumber;
}

// CONSTRUCTORS
ComputerClub::ComputerClub() noexcept
{
    // input event types
    eventHandlers[INPUT_CLIENT_ARRIVES] = &ComputerClub::inputClientArrives;
    eventHandlers[INPUT_CLIENT_SITS] = &ComputerClub::inputClientSits;
    eventHandlers[INPUT_CLIENT_WAITS] = &ComputerClub::inputClientWaits;
    eventHandlers[INPUT_CLIENT_LEAVES] = &ComputerClub::inputClientLeaves;
    // output event types
    eventHandlers[OUTPUT_CLIENT_LEAVES] = &ComputerClub::outputClientLeaves;
    eventHandlers[OUTPUT_CLIENT_SITS] = &ComputerClub::outputClientSits;
    eventHandlers[OUTPUT_ERROR_EVENT] = &ComputerClub::outputErrorEvent;
}

// GETTERS

int ComputerClub::getTablesNumber() const noexcept { return tablesNumber; }

int ComputerClub::getFreeTables() const noexcept { return freeTables; }

int ComputerClub::getHourlyRate() const noexcept { return Table::hourlyRate; }

std::string ComputerClub::getOpeningTime() const noexcept { return openingTime; }

std::string ComputerClub::getClosingTime() const noexcept { return closingTime; }

std::vector<Table> ComputerClub::getTables() const noexcept { return tables; }

std::list<Event> ComputerClub::getEvents() const noexcept { return events; }

// READING INPUT FUNCTIONS

void ComputerClub::readInitialParameters(std::ifstream &file)
{
    file >> tablesNumber >> openingTime >> closingTime >> Table::hourlyRate;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    tables.resize(tablesNumber + 1);
    for (int id = 1; id <= tablesNumber; ++id) {
        tables[id].number = id;
    }
    freeTables = tablesNumber;
}

void ComputerClub::readEvents(std::vector<std::string> &inputLines)
{
    Event event;
    for (const auto &line : inputLines) {
        std::istringstream iss(line);
        std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                        std::istream_iterator<std::string>());
        event.time = tokens[0];
        event.type = std::stoi(tokens[1]);
        event.body = tokens[2];
        event.tableNumber =
            tokens.size() == TOKENS_IN_SITS_EVENT ? std::stoi(tokens[3]) : NOT_AT_TABLE;
        events.push_back(event);
    }
}

void ComputerClub::readInput(const std::string &filename, std::vector<std::string> &inputLines)
{
    std::ifstream file(filename);
    if (!file) {
        throw FileOpenError(filename);
    }

    readInitialParameters(file);

    std::string line;
    std::string previousEventTime = "";
    while (std::getline(file, line)) {
        if (!Validator::isValidLine(line, tablesNumber, previousEventTime)) {
            throw InputFormatError(line);
        }
        inputLines.push_back(line);
    }
    readEvents(inputLines);
    file.close();
}

// PROCESSING FUNCTIONS

void ComputerClub::processEvent(std::list<Event>::iterator &eventIterator)
{
    (this->*eventHandlers[eventIterator->type])(*eventIterator);
    if (newEventGenerated) {
        events.insert(std::next(eventIterator), newEvent);
        newEventGenerated = false;
        ++eventIterator;
    }
}

void ComputerClub::processEvents()
{
    auto eventIterator = events.begin();
    while (eventIterator != events.end()) {
        int eventTimeInMinutes = Validator::convertTimeToMinutes(eventIterator->time);
        int closingTimeInMinutes = Validator::convertTimeToMinutes(closingTime);

        // If the event time is after the closing time, process end of day events
        if (closingTimeInMinutes < eventTimeInMinutes) {
            endOfDayEvents();
            break;
        }

        processEvent(eventIterator);
        ++eventIterator;
    }

    // Process the remaining events
    while (eventIterator != events.end()) {
        processEvent(eventIterator);
        ++eventIterator;
    }
}

void ComputerClub::endOfDayEvents()
{
    auto eventIterator = events.begin();

    // Skip all events before the closing time
    while (eventIterator != events.end()
           && Validator::convertTimeToMinutes(eventIterator->time) <= Validator::convertTimeToMinutes(closingTime)) {
        ++eventIterator;
    }

    while (!clientTableMap.empty()) {
        auto client = clientTableMap.begin();
        Event event{closingTime, OUTPUT_CLIENT_LEAVES, client->first};
        events.insert(eventIterator, event);
        (this->*eventHandlers[event.type])(event);
    }
}

bool ComputerClub::checkAndHandleError(bool condition, Event &event, const std::string &errorMessage)
{
    if (!condition) {
        Event errorEvent{event.time, OUTPUT_ERROR_EVENT, errorMessage};
        (this->*eventHandlers[OUTPUT_ERROR_EVENT])(errorEvent);
        return true;
    }
    return false;
}

// HELPER FUNCTIONS
std::string formatTime(int totalMinutes)
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << totalMinutes / 60 << ":";
    oss << std::setw(2) << std::setfill('0') << totalMinutes % 60;
    return oss.str();
}

void ComputerClub::setTable(int tableNumber, Event &event)
{
    tables[tableNumber].isOccupied = true;
    tables[tableNumber].clientName = event.body;
    tables[tableNumber].startTime = event.time;
    clientTableMap[event.body] = event.tableNumber;
}

// WRITING OUTPUT FUNCTIONS

void ComputerClub::writeOutput()
{
    std::cout << openingTime << std::endl;

    for (const auto &event : events) {
        std::cout << event.time << " " << event.type << " " << event.body << " "
                  << (event.tableNumber == NOT_AT_TABLE ? "" : std::to_string(event.tableNumber))
                  << std::endl;
    }
    std::cout << closingTime << std::endl;

    for (const auto &table : tables) {
        if (table.number != 0) {  // Skip the first table as it's not used
            std::cout << table.number << " " << table.totalRevenue << " ";
            std::cout << formatTime(table.totalMinutesOccupied) << std::endl;
        }
    }
}

// INPUT EVENTS

void ComputerClub::inputClientArrives(Event &event)
{
    bool clientExists = clientTableMap.find(event.body) != clientTableMap.end();
    if (checkAndHandleError(!clientExists, event, "YouShallNotPass"))
        return;

    int eventTimeInMinutes = Validator::convertTimeToMinutes(event.time);
    int openingTimeInMinutes = Validator::convertTimeToMinutes(openingTime);
    int closingTimeInMinutes = Validator::convertTimeToMinutes(closingTime);

    bool isClubOpen =
        openingTimeInMinutes <= eventTimeInMinutes && eventTimeInMinutes <= closingTimeInMinutes;
    if (checkAndHandleError(isClubOpen, event, "NotOpenYet"))
        return;

    clientTableMap[event.body] = NOT_AT_TABLE;
}

void ComputerClub::inputClientSits(Event &event)
{
    if (checkAndHandleError(clientTableMap.find(event.body) != clientTableMap.end(), event, "ClientUnknown"))
        return;
    if (checkAndHandleError(!tables[event.tableNumber].isOccupied, event, "PlaceIsBusy"))
        return;

    if (clientTableMap[event.body] != NOT_AT_TABLE) {
        freeTable(clientTableMap[event.body], event.time);
    }
    setTable(event.tableNumber, event);
    freeTables--;
}

void ComputerClub::inputClientWaits(Event &event)
{
    if (checkAndHandleError(freeTables <= 0, event, "ICanWaitNoLonger!"))
        return;

    if (tablesNumber < waitingQueue.size()) {
        (this->*eventHandlers[OUTPUT_CLIENT_LEAVES])(event);
        return;
    }

    waitingQueue.push(event.body);
}

void ComputerClub::deleteLeftClientFromQueue(std::string &clientName)
{
    std::queue<std::string> tempQueue;
    while (!waitingQueue.empty()) {
        if (waitingQueue.front() != clientName) {
            tempQueue.push(waitingQueue.front());
        }
        waitingQueue.pop();
    }
    waitingQueue = tempQueue;
}

void ComputerClub::inputClientLeaves(Event &event)
{
    std::string clientName = event.body;
    if (checkAndHandleError(clientTableMap.find(clientName) != clientTableMap.end(), event, "ClientUnknown"))
        return;

    int tableNumber = clientTableMap[event.body];
    clientTableMap.erase(event.body);
    if (tableNumber == NOT_AT_TABLE) {
        return;
    }
    freeTable(tableNumber, event.time);

    // Check if the leaving client is in the waiting queue and remove them
    deleteLeftClientFromQueue(clientName);

    if (!waitingQueue.empty()) {
        std::string nextClient = waitingQueue.front();
        waitingQueue.pop();
        Event nextEvent{event.time, event.type, nextClient, tableNumber};
        (this->*eventHandlers[OUTPUT_CLIENT_SITS])(nextEvent);
    }
}

// OUTPUT EVENTS

void ComputerClub::handleClientEvent(Event &event, int eventType)
{
    newEventGenerated = true;
    setNewEvent(event.time, eventType, event.body, event.tableNumber);
    int tableNumber = clientTableMap[event.body];
    if (tableNumber != NOT_AT_TABLE) {
        freeTable(tableNumber, event.time);
    }
    if (eventType == OUTPUT_CLIENT_SITS) {
        setTable(event.tableNumber, event);
        tables[event.tableNumber].number = event.tableNumber;
    }
    if (eventType == OUTPUT_CLIENT_LEAVES) {
        clientTableMap.erase(event.body);
        deleteLeftClientFromQueue(event.body);
    }
}

void ComputerClub::outputClientLeaves(Event &event)
{
    handleClientEvent(event, OUTPUT_CLIENT_LEAVES);
}

void ComputerClub::outputClientSits(Event &event) { handleClientEvent(event, OUTPUT_CLIENT_SITS); }

void ComputerClub::outputErrorEvent(Event &event)
{
    newEventGenerated = true;
    setNewEvent(event.time, OUTPUT_ERROR_EVENT, event.body);
}

// OTHER FUNCTIONS

void ComputerClub::freeTable(int tableNumber, std::string &time)
{
    Table &table = tables[tableNumber];
    table.isOccupied = false;
    int minutesOccupied = table.countMinutesOccupied(table.startTime, time);
    table.totalMinutesOccupied += minutesOccupied;
    table.totalRevenue += table.countRevenue(minutesOccupied);
    freeTables++;
}