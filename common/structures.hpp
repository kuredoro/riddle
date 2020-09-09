#pragma once
#include <cstddef>

// For common data structures


/**
 * Represents the position of a character in a source file
 */
struct position_t
{
    size_t line;       // starting at 1
    size_t column;     // starting at 1, in bytes
    bool operator==(const position_t& other) const {
        return line == other.line && column == other.column;
    }
};
