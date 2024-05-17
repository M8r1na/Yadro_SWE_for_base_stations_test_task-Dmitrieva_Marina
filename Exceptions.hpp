#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

struct ComputerClubError : public std::runtime_error {
    explicit ComputerClubError(const std::string &message)
        : std::runtime_error(message)
    {
    }
};

struct InvalidNumberOfArgumentsError : public ComputerClubError {
    explicit InvalidNumberOfArgumentsError(const int arguments)
        : ComputerClubError("Invalid number of arguments: expected " + std::to_string(arguments))
    {
    }
};

struct FileOpenError : public ComputerClubError {
    explicit FileOpenError(const std::string &filename)
        : ComputerClubError("Could not open file " + filename)
    {
    }
};

struct InputFormatError : public ComputerClubError {
    explicit InputFormatError(const std::string &message)
        : ComputerClubError("Invalid input format: " + message)
    {
    }
};

#endif  // EXCEPTIONS_HPP