cmake_minimum_required(VERSION 3.10)
project(ComputerClub)

set(CMAKE_CXX_STANDARD 17)

add_executable(ComputerClub
    main.cpp
    ComputerClub.cpp
    ComputerClubEvent.cpp
    ComputerClubTable.cpp
    ComputerClubValidator.cpp
)

target_include_directories(ComputerClub PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})