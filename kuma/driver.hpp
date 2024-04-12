#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <ctype.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace core {
inline FILE* in;
inline FILE* out;
inline FILE* err;

inline int lines = 1;
}  // namespace core

namespace lexer {
bool scan();
void put_token(std::string s, int symbol);
int check_keyword_type(std::string s);
void put_error(std::string s, int type);
}  // namespace lexer

namespace parser {
/* tools */
bool parser();
void parse_next_token();
bool match(int symbol);
void put_error(int type);

/* states */
bool program();
bool sub_program();

bool declaration_table();
bool declaration_table_prime();
bool declaration();

bool variable_declaration();
bool variable();
bool function_declaration();
bool parameter();
bool function_body();

bool execution_table();
bool execution_table_prime();
bool execution();

bool read_statement();
bool write_statement();
bool assignment();

bool expression();
bool expression_prime();

bool item();
bool item_prime();

bool factor();
bool condition();
bool condition_expression();
bool relational_operator();
bool function_call();
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
    /* lexer error */
    SYNTAX_ERROR = 255,
    INVALID_LENGTH_ERROR = 256,
    INVALID_ASSIGN_ERROR = 257,

    /* parser error */
    MISSING_BEGIN_ERROR = 258,
    MISSING_END_ERROR = 259,
    MISSING_SEMICOLON_ERROR = 260,
    MISSING_INTEGER_ERROR = 261,
    MISSING_IDENTIFIER_ERROR = 262,
    MISSING_FUNCTION_ERROR = 263,
    MISSING_ROUND_BRACKET_ERROR = 264,
    ILLEGAL_PARAMETER_ERROR = 265,
    BAD_DECLARATION_ERROR = 266,
    BAD_EXECUTION_ERROR = 267,

};

#endif