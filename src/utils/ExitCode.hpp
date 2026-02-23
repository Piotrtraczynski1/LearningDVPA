#pragma once

#include <cstdint>
#include <iostream>

enum class ExitCode : std::uint8_t
{
    OK = 0,
    UNDEFINED = 1,
    COUNTEREXAMPLE = 2,
    STACKCONTENTDIVERGE = 3,
    EQUIVALENCEQUERY = 4,
    GENERATOR = 5,
    TIMEOUT = 6
};

constexpr std::uint8_t toExit(ExitCode ec)
{
    return static_cast<std::uint8_t>(ec);
}

constexpr const char *toString(ExitCode ec)
{
    switch (ec)
    {
    case ExitCode::OK:
        return "OK";
    case ExitCode::UNDEFINED:
        return "UNDEFINED";
    case ExitCode::COUNTEREXAMPLE:
        return "COUNTEREXAMPLE";
    case ExitCode::STACKCONTENTDIVERGE:
        return "STACKCONTENTDIVERGE";
    case ExitCode::EQUIVALENCEQUERY:
        return "EQUIVALENCEQUERY";
    case ExitCode::GENERATOR:
        return "GENERATOR";
    case ExitCode::TIMEOUT:
        return "TIMEOUT";
    }
    return "UNKNOWN_EXIT_CODE";
}