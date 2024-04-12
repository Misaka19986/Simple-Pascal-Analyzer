#include "driver.hpp"

/*
 * token[0]: the token name
 * token[1]: the type number
 * token[2]: token location
 */
std::vector<std::string> token;

bool parser::parser() {
    printf("\nparser begin\n");
    fprintf(core::err, "--parser\n");

    if (program())
        return true;
    else
        return false;
}

void parser::parse_next_token() {
loop:
    char* temp;
    size_t len = 0;

    token.clear();

    getline(&temp, &len, core::in);

    int i = 0;
    for (auto index = 0; index < 3; ++index) {
        std::string temp_string = "";
        while (temp[i] == ' ')
            ++i;
        while (temp[i] != ' ' && temp[i] != '\n') {
            temp_string += temp[i];
            ++i;
        }
        token.push_back(temp_string);
    }

    if (atoi(token[1].c_str()) == EOLN) {  // skip '\n'
        free(temp);
        goto loop;
    }
}

bool parser::match(int symbol) {
    if (atoi(token[1].c_str()) == symbol) {
        return true;
    } else
        return false;
}

void parser::put_error(int type) {
    printf("putting an error :%d\n", type);

    switch (type) {
        case MISSING_BEGIN_ERROR:
            fprintf(core::err, "missing begin: getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case MISSING_END_ERROR:
            fprintf(core::err, "missing end: getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case MISSING_SEMICOLON_ERROR:
            fprintf(core::err, "missing ';': getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case MISSING_INTEGER_ERROR:
            fprintf(core::err,
                    "missing the type of variable: getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case MISSING_IDENTIFIER_ERROR:
            fprintf(
                core::err,
                "missing the name of variable or function: getting '%s', at "
                "line %s\n",
                token[0].c_str(), token[2].c_str());
            break;
        case MISSING_FUNCTION_ERROR:
            fprintf(core::err, "missing 'function': getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case MISSING_ROUND_BRACKET_ERROR:
            fprintf(core::err, "missing '(' or ')': getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case ILLEGAL_PARAMETER_ERROR:
            fprintf(core::err, "illegal parameter statements: at line %s\n",
                    token[2].c_str());
            break;
        case BAD_DECLARATION_ERROR:
            fprintf(core::err, "illegal declaration statements: at line %s\n",
                    token[2].c_str());
            break;
        case BAD_EXECUTION_ERROR:
            fprintf(core::err, "illegal execution statements: at line %s\n",
                    token[2].c_str());
            break;
    }
}

bool parser::program() {
    printf("program\n\n");

    parse_next_token();
    if (sub_program())
        return true;
    else
        return false;
}

bool parser::sub_program() {
    printf("sub_program\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(BEGIN)) {
        parse_next_token();
        if (declaration_table()) {
            // to check declaration is ended, we have move to next token behind
            // ';'
            // so we dont check ';'
            if (execution_table()) {
                if (match(END)) {
                    return true;
                } else {
                    put_error(MISSING_END_ERROR);
                    goto bad;
                }
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else {
        put_error(MISSING_BEGIN_ERROR);
        goto bad;
    }

bad:
    return false;
}

bool parser::declaration_table() {
    printf("declaration_table\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (declaration()) {
        parse_next_token();
        if (declaration_table_prime()) {
            return true;
        } else {
            goto bad;
        }
    } else {
        put_error(BAD_DECLARATION_ERROR);
        goto bad;
    }

bad:
    return false;
}

bool parser::declaration_table_prime() {
    printf("declaration_table_prime\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(SEMICOLON)) {
        parse_next_token();
        if (declaration()) {
            if (declaration_table_prime()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            return true;
            // matching the ';' already prove legal
            // if there is something behind the ';' and it's not declaration
            // then we have left the scope, leave the token to next checker
        }
    } else if (match(END)) {
        return true;  // no more declaration
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::declaration() {
    printf("declaration\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (variable_declaration()) {
        return true;
    } else if (function_declaration()) {
        return true;
    } else if (match(SEMICOLON)) {
        return true;  // declaration is blank
    } else if (match(END)) {
        return true;  // declaration is blank
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::variable_declaration() {
    printf("variable_declaration\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(INTEGER)) {
        parse_next_token();
        if (variable()) {
            return true;
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::variable() {
    printf("variable\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(IDENTIFIER)) {
        return true;
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::function_declaration() {
    printf("function_declaration\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(INTEGER)) {
        parse_next_token();
        if (match(FUNCTION)) {
            parse_next_token();
            if (match(IDENTIFIER)) {
                parse_next_token();
                if (match(LEFT_ROUND_BRACKET)) {
                    parse_next_token();
                    if (parameter()) {  // parameter can be nothing
                        parse_next_token();
                        if (match(RIGHT_ROUND_BRACKET)) {
                            parse_next_token();
                            if (match(SEMICOLON)) {
                                parse_next_token();
                                if (function_body()) {
                                    return true;
                                } else {
                                    goto bad;
                                }
                            } else {
                                put_error(MISSING_SEMICOLON_ERROR);
                                goto bad;
                            }
                        } else {
                            put_error(MISSING_ROUND_BRACKET_ERROR);
                            goto bad;
                        }
                    } else if (match(RIGHT_ROUND_BRACKET)) {  // parameter is
                                                              // blank
                        parse_next_token();
                        if (match(SEMICOLON)) {
                            parse_next_token();
                            if (function_body()) {
                                return true;
                            } else {
                                goto bad;
                            }
                        } else {
                            put_error(MISSING_SEMICOLON_ERROR);
                            goto bad;
                        }
                    } else {
                        put_error(MISSING_ROUND_BRACKET_ERROR);
                        goto bad;
                    }
                } else {
                    put_error(MISSING_ROUND_BRACKET_ERROR);
                    goto bad;
                }
            } else {
                put_error(MISSING_IDENTIFIER_ERROR);
                goto bad;
            }
        } else {
            put_error(MISSING_FUNCTION_ERROR);
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::parameter() {
    printf("parameter\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (variable()) {
        return true;
    } else {
        put_error(ILLEGAL_PARAMETER_ERROR);
        goto bad;
    }
bad:
    return false;
}

bool parser::function_body() {
    printf("function_body\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(BEGIN)) {
        parse_next_token();
        if (declaration_table()) {
            if (execution_table()) {
                if (match(END)) {
                    return true;
                } else {
                    put_error(MISSING_END_ERROR);
                    goto bad;
                }
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else {
        put_error(MISSING_BEGIN_ERROR);
        goto bad;
    }
bad:
    return false;
}

bool parser::execution_table() {
    printf("execution_table\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (execution()) {
        if (execution_table_prime()) {
            return true;
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::execution_table_prime() {
    printf("execution_table_prime\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(SEMICOLON)) {
        parse_next_token();
        if (execution()) {
            if (execution_table_prime()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            return true;
            // matching the ';' already prove legal
            // if there is something behind the ';' and it's not execution
            // then we have left the scope, leave the token to next checker
        }
    } else if (match(END)) {
        return true;
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::execution() {
    printf("execution\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (read_statement()) {
        return true;
    } else if (write_statement()) {
        return true;
    } else if (assignment()) {
        return true;
    } else if (condition()) {
        return true;
    } else if (match(SEMICOLON)) {
        return true;  // execution is blank
    } else if (match(END)) {
        return true;  // execution is blank
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::read_statement() {
    printf("read_statement\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(READ)) {
        parse_next_token();
        if (match(LEFT_ROUND_BRACKET)) {
            parse_next_token();
            if (match(RIGHT_ROUND_BRACKET)) {
                return true;
            } else {
                put_error(MISSING_ROUND_BRACKET_ERROR);
                goto bad;
            }
        } else {
            put_error(MISSING_ROUND_BRACKET_ERROR);
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::write_statement() {
    printf("write_statement\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(WRITE)) {
        parse_next_token();
        if (match(LEFT_ROUND_BRACKET)) {
            parse_next_token();
            if (match(RIGHT_ROUND_BRACKET)) {
                return true;
            } else {
                put_error(MISSING_ROUND_BRACKET_ERROR);
                goto bad;
            }
        } else {
            put_error(MISSING_ROUND_BRACKET_ERROR);
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::assignment() {
    printf("assignment\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (variable()) {
        parse_next_token();
        if (match(ASSIGN)) {
            parse_next_token();
            if (expression()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::expression() {
    printf("expression\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (item()) {
        if (expression_prime()) {
            return true;
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::expression_prime() {
    printf("expression_prime\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(MINUS)) {
        parse_next_token();
        if (item()) {
            if (expression_prime()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::item() {
    printf("item\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (factor()) {
        parse_next_token();
        if (item_prime()) {
            return true;
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::item_prime() {
    printf("item_prime\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(TIMES)) {
        parse_next_token();
        if (factor()) {
            parse_next_token();
            if (item_prime()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else if (match(MINUS)) {
        // no more factor
        return true;
    } else if (match(SEMICOLON)) {
        // no more factor
        return true;
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::factor() {
    printf("factor\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (variable()) {
        return true;
    } else if (match(CONST_NUM)) {
        return true;
    } else if (function_call()) {
        return true;
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::condition() {
    printf("condition\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(IF)) {
        parse_next_token();
        if (condition_expression()) {
            parse_next_token();
            if (match(THEN)) {
                parse_next_token();
                if (execution()) {
                    parse_next_token();
                    if (match(ELSE)) {
                        parse_next_token();
                        if (execution()) {
                            return true;
                        } else {
                            goto bad;
                        }
                    } else {
                        goto bad;
                    }
                } else {
                    goto bad;
                }
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::condition_expression() {
    printf("condition_expression\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (expression()) {
        parse_next_token();
        if (relational_operator()) {
            parse_next_token();
            if (expression()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::relational_operator() {
    printf("relational_operator\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(LESS_THAN)) {
        return true;
    } else if (match(LESS_EQUAL)) {
        return true;
    } else if (match(GREATER_THAN)) {
        return true;
    } else if (match(GREATER_EQUAL)) {
        return true;
    } else if (match(EQUAL)) {
        return true;
    } else if (match(NOT_EQUAL)) {
        return true;
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::function_call() {
    printf("function_call\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(IDENTIFIER)) {
        parse_next_token();
        if (match(LEFT_ROUND_BRACKET)) {
            parse_next_token();
            if (variable()) {
                parse_next_token();
                if (match(RIGHT_ROUND_BRACKET)) {
                    return true;
                } else {
                    put_error(MISSING_ROUND_BRACKET_ERROR);
                    goto bad;
                }
            } else if (match(RIGHT_ROUND_BRACKET)) {
                // there is no parameter
                return true;
            } else {
                goto bad;
            }
        } else {
            put_error(MISSING_ROUND_BRACKET_ERROR);
            goto bad;
        }
    } else {
        goto bad;
    }

bad:
    return false;
}