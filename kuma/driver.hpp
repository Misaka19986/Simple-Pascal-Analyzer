#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <ctype.h>
#include <iostream>
#include <regex>
#include <string>

namespace core {
inline FILE* in;
inline FILE* out;
inline FILE* err;

inline int lines = 1;
}  // namespace core

namespace lexer {
void scan();
void put_token(std::string s, int symbol);
int check_keyword_type(std::string s);
void put_error(std::string s, int type);
}  // namespace lexer

namespace parser {
bool parser();
bool program();
bool sub_program();
bool declaration_table();
bool declaration();
bool variable_declaration();
bool variable();
bool identifier();
bool function_declaration();
bool parameter();
bool function_body();
bool execution_table();
bool execution();
bool read_statement();
bool write_statement();
bool expression();
bool item();
bool factor();
bool constant();
bool condition();
bool condition_expression();
bool relational_operator();
}  // namespace parser

inline std::string keywords[] = {
    "begin", "end",      "integer", "if",    "then",
    "else",  "function", "read",    "write",
};

enum Symbols {
    /* keywords */
    BEGIN = 1,
    END = 2,
    INTEGER = 3,
    IF = 4,
    THEN = 5,
    ELSE = 6,
    FUNCTION = 7,
    READ = 8,
    WRITE = 9,

    /* others */
    IDENTIFIER = 10,
    CONST_NUM = 11,

    /* symbols */
    EQUAL = 12,                // =
    NOT_EQUAL = 13,            // <>
    LESS_EQUAL = 14,           // <=
    LESS_THAN = 15,            // <
    GREATER_EQUAL = 16,        // >=
    GREATER_THAN = 17,         // >
    MINUS = 18,                // -
    TIMES = 19,                // *
    ASSIGN = 20,               // :=
    LEFT_ROUND_BRACKET = 21,   // (
    RIGHT_ROUND_BRACKET = 22,  // )
    SEMICOLON = 23,            // ;
    EOLN = 24,                 // \n
    EOF_ = 25,                 // Avoid confliction
};

enum Errors {
    // Error codes
    SYNTAX_ERROR = 255,
    INVALID_LENGTH_ERROR = 256,
    INVALID_ASSIGN_ERROR = 257,
};

#endif