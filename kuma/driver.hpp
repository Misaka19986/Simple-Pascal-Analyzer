#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <iostream>
#include <string>
#include <regex>

namespace core {
    inline FILE* in;
    inline FILE* out;
    inline FILE* err;

    inline int lines = 1;
}


namespace lexer {
    void scan();
    void put_token(std::string s, int symbol);
    int  check_type(std::string s);
    void put_error(std::string s, int type);
}

namespace parser {

}

enum Symbols {
    BEGIN = 1,
    END = 2,
    INTEGER = 3,
    IF = 4,
    THEN = 5,
    ELSE = 6,
    FUNCTION = 7,
    READ = 8,
    WRITE = 9,
    IDENTIFIER = 10,
    CONST_NUM = 11,
    EQUAL = 12, // =
    NOT_EQUAL = 13, // <>
    LESS_EQUAL = 14,    // <=
    LESS_THAN = 15, // <
    GREATER_EQUAL = 16, // >=
    GREATER_THAN = 17,  // >
    MINUS = 18, // -
    TIMES = 19, // *
    ASSIGN = 20,    // :=
    LEFT_ROUND_BRACKET = 21,    // (
    RIGHT_ROUND_BRACKET = 22,   // )
    SEMICOLON = 23, // ;
    EOLN = 24,  // \n
    EOF_ = 25,   // Avoid confliction
};

enum Errors {
    // Error codes
    SYNTAX_ERROR = 255,
    INVALID_LENGTH_ERROR = 256,
    INVALID_ASSIGN_ERROR = 257,
};

#endif