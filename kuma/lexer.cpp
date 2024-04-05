#include "driver.hpp"

/**
 * @brief A main function to scan a character everytime from input file.
 * Check if the character forms a valid token, and do output.
 *
 */
void lexer::scan() {
    std::string token = "";

    char ch = fgetc(core::in);
    while (!feof(core::in)) {
        if (' ' == ch || '\t' == ch || '\r' == ch) {
            ch = fgetc(core::in);
        } else if ('\n' == ch) {
            core::lines++;
            put_token("EOLN", EOLN);
            ch = fgetc(core::in);
        } else if (isdigit(ch)) {
            // It may be a constant
            // leave it to parser to judge if it is legal
            token.clear();
            while (isdigit(ch)) {
                token += ch;
                ch = fgetc(core::in);
            }

            put_token(token, CONST_NUM);
        } else if (isalpha(ch) || '_' == ch) {
            // It may be a identifier or a keyword
            token.clear();
            while (isalpha(ch) || isdigit(ch) || '_' == ch) {
                token += ch;
                ch = fgetc(core::in);
            }

            int symbol = check_keyword_type(token);

            if (token.length() > 16)
                put_error(token, INVALID_LENGTH_ERROR);
            else
                put_token(token, symbol);
        } else {
            token.clear();
            token += ch;

            int symbol = SYNTAX_ERROR;

            switch (ch) {
                case '=':
                    ch = fgetc(core::in);
                    symbol = EQUAL;
                    break;
                case '<':
                    ch = fgetc(core::in);
                    if ('>' == ch) {
                        token += ch;
                        symbol = NOT_EQUAL;
                        ch = fgetc(core::in);
                    } else if ('=' == ch) {
                        token += ch;
                        symbol = LESS_EQUAL;
                        ch = fgetc(core::in);
                    } else {
                        symbol = LESS_THAN;
                    }

                    break;
                case '>':
                    ch = fgetc(core::in);
                    if ('=' == ch) {
                        token += ch;
                        symbol = GREATER_EQUAL;
                        ch = fgetc(core::in);
                    } else {
                        symbol = GREATER_THAN;
                    }

                    break;
                case '-':
                    ch = fgetc(core::in);
                    symbol = MINUS;
                    break;
                case '*':
                    ch = fgetc(core::in);
                    symbol = TIMES;
                    break;
                case ':':
                    ch = fgetc(core::in);
                    if ('=' == ch) {
                        token += ch;
                        symbol = ASSIGN;
                        ch = fgetc(core::in);
                    } else {
                        symbol = INVALID_ASSIGN_ERROR;
                    }
                    break;
                case '(':
                    ch = fgetc(core::in);
                    symbol = LEFT_ROUND_BRACKET;
                    break;
                case ')':
                    ch = fgetc(core::in);
                    symbol = RIGHT_ROUND_BRACKET;
                    break;
                case ';':
                    ch = fgetc(core::in);
                    symbol = SEMICOLON;
                    break;
                default:
                    ch = fgetc(core::in);
                    break;
            }

            if (symbol >= 255)
                put_error(token, symbol);
            else
                put_token(token, symbol);
        }
    }

    put_token("EOF", EOF_);

    return;
}

/**
 * @brief A function to put a formatted output into output file.
 *
 * @param s The name of the token
 * @param symbol The number of that symbol, check driver.hpp for more
 *
 */
void lexer::put_token(std::string s, int symbol) {
    printf("putting a token %s:%d\n", s.c_str(), symbol);

    fprintf(core::out, "%16s %d\n", s.c_str(), symbol);
}

/**
 * @brief A function to check the token type.
 *
 * @param s The token
 *
 * @return The corresponding number of the token, -1 as error
 *
 */
int lexer::check_keyword_type(std::string s) {
    for (auto i = 0; i < keywords->length(); ++i) {
        if (s == keywords[i]) {
            return i + 1;
        }
    }

    return IDENTIFIER;
}

/**
 * @brief A function to put a formatted output into error file.
 *
 * @param s The word that caused an error
 * @param type The error type
 *
 */
void lexer::put_error(std::string s, int type) {
    printf("putting an error %s:%d\n", s.c_str(), type);

    switch (type) {
        case SYNTAX_ERROR:
            fprintf(core::err, "syntax error: '%s', at line %d\n", s.c_str(),
                    core::lines);
            break;
        case INVALID_LENGTH_ERROR:
            fprintf(
                core::err,
                "identifier or constant is too long: '%.10s'..., at line %d\n",
                s.c_str(), core::lines);
            break;
        case INVALID_ASSIGN_ERROR:
            fprintf(core::err, "invalid assignment: missing '=', at line %d\n",
                    core::lines);
            break;
    }
}
