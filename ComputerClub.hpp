#ifndef COMPUTER_CLUB_HPP
#define COMPUTER_CLUB_HPP

#include "ComputerClubEvent.hpp"
#include "ComputerClubTable.hpp"
#include <fstream>
#include <list>
#include <map>
#include <queue>
#include <string>
#include <vector>

const int NOT_AT_TABLE = -1;

// FORWARD DECLARATION

class ComputerClub
{
    // INTEGER VALUES
    int tablesNumber;
    int freeTables;
    int hourlyRate;
    // STRING VALUES
    std::string openingTime;
    std::string closingTime;
    // OBJECTS
    std::vector<Table> tables;
    std::list<Event> events;
    std::queue<std::string> waitingQueue;
    std::map<std::string, int> clientTableMap;

    using EventFunction = void (ComputerClub::*)(Event &);
    std::map<int, EventFunction> eventHandlers;

  public:
    // GENERATED EVENT
    Event newEvent;
    void setNewEvent(std::string time, int type, std::string body, int tableNumber);
    bool newEventGenerated = false;

    // CONSTRUCTORS
    ComputerClub() noexcept;

    // GETTERS
    [[nodiscard]]int getTablesNumber() const noexcept;
    [[nodiscard]]int getFreeTables() const noexcept;
    [[nodiscard]]int getHourlyRate() const noexcept;
    [[nodiscard]]std::string getOpeningTime() const noexcept;
    [[nodiscard]]std::string getClosingTime() const noexcept;
    [[nodiscard]]std::vector<Table> getTables() const noexcept;
    [[nodiscard]]std::list<Event> getEvents() const noexcept;

    // READING INPUT FUNCTIONS
    void readInput(const std::string &filename, std::vector<std::string> &inputLines);
    void readInitialParameters(std::ifstream &file);
    void readEvents(std::vector<std::string> &inputLines);

    // PROCESSING FUNCTIONS
    void processEvent(std::list<Event>::iterator& eventIterator);
    void processEvents();
    void endOfDayEvents();
    bool checkAndHandleError(bool condition, Event &event, const std::string &errorMessage);

    // WRITING OUTPUT FUNCTIONS
    void writeOutput();

    // INPUT EVENTS
    void inputClientArrives(Event &event);
    void inputClientSits(Event &event);
    void deleteLeftClientFromQueue(std::string &clientName);
    void inputClientWaits(Event &event);
    void inputClientLeaves(Event &event);

    // OUTPUT EVENTS
    void handleClientEvent(Event &event, int eventType);
    void outputClientLeaves(Event &event);
    void outputClientSits(Event &event);
    void outputErrorEvent(Event &event);

    // OTHER FUNCTIONS
    void freeTable(int tableNumber, std::string &time);
    void setTable(int tableNumber, Event &event);
};

#endif  // COMPUTER_CLUB_HPP