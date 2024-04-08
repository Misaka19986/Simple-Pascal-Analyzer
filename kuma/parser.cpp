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
        goto loop;
    }
}

bool parser::match(int symbol) {
    parse_next_token();
    if (atoi(token[1].c_str()) == symbol)
        return true;
    else
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
        case ILLEGAL_EXECUTION_ERROR:
            fprintf(core::err, "illegal execution statements: at line %s\n",
                    token[2].c_str());
            break;
    }
}

bool parser::program() {
    printf("program\n");
    if (sub_program())
        return true;
    else
        return false;
}

bool parser::sub_program() {
    printf("sub_program\n");
    if (match(BEGIN)) {
        if (declaration_table()) {
            if (match(SEMICOLON)) {
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
                put_error(MISSING_SEMICOLON_ERROR);
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
    if (declaration()) {
        return true;
    } else if (declaration_table()) {
        if (match(SEMICOLON)) {
            if (declaration()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            put_error(MISSING_SEMICOLON_ERROR);
            goto bad;
        }
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::declaration() {
    printf("declaration\n");
    if (variable_declaration()) {
        return true;
    } else if (function_declaration()) {
        return true;
    } else {
        goto bad;
    }

bad:
    return false;
}

bool parser::variable_declaration() {
    printf("variable_declaration\n");
    if (match(INTEGER)) {
        if (variable()) {
            return true;
        } else {
            goto bad;
        }
    } else {
        put_error(MISSING_INTEGER_ERROR);
        goto bad;
    }

bad:
    return false;
}

bool parser::variable() {
    printf("variable\n");
    if (match(IDENTIFIER)) {
        return true;
    } else {
        put_error(MISSING_IDENTIFIER_ERROR);
        goto bad;
    }
bad:
    return false;
}

bool parser::function_declaration() {
    printf("function_declaration\n");
    if (match(INTEGER)) {
        if (match(FUNCTION)) {
            if (match(IDENTIFIER)) {
                if (match(LEFT_ROUND_BRACKET)) {
                    if (parameter()) {  // parameter can be nothing
                        if (match(RIGHT_ROUND_BRACKET)) {
                            if (match(SEMICOLON)) {
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
                    } else if (match(RIGHT_ROUND_BRACKET)) {
                        if (match(SEMICOLON)) {
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
        put_error(MISSING_INTEGER_ERROR);
        goto bad;
    }
bad:
    return false;
}

bool parser::parameter() {
    printf("parameter\n");
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
    if (match(BEGIN)) {
        if (declaration_table()) {
            if (match(SEMICOLON)) {
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
                put_error(MISSING_SEMICOLON_ERROR);
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
    if (execution()) {
        return true;
    } else if (execution_table()) {
        if (match(SEMICOLON)) {
            if (execution()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            put_error(MISSING_SEMICOLON_ERROR);
            goto bad;
        }
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::execution() {
    printf("execution\n");
    if (read_statement()) {
        return true;
    } else if (write_statement()) {
        return true;
    } else if (assignment()) {
        return true;
    } else if (condition()) {
        return true;
    } else {
        put_error(ILLEGAL_EXECUTION_ERROR);
        goto bad;
    }
bad:
    return false;
}

bool parser::read_statement() {
    printf("read_statement\n");
    if (match(READ)) {
        if (match(LEFT_ROUND_BRACKET)) {
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
    if (match(WRITE)) {
        if (match(LEFT_ROUND_BRACKET)) {
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
    if (variable()) {
        if (match(ASSIGN)) {
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
    if (expression()) {
        if (match(MINUS)) {
            if (item()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else if (item()) {
        return true;
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::item() {
    printf("item\n");
    if (item()) {
        if (match(TIMES)) {
            if (factor()) {
                return true;
            } else {
                goto bad;
            }
        } else {
            goto bad;
        }
    } else if (item()) {
        return true;
    } else {
        goto bad;
    }
bad:
    return false;
}

bool parser::factor() {
    printf("factor\n");
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
    if (match(IF)) {
        if (condition_expression()) {
            if (match(THEN)) {
                if (execution()) {
                    if (match(ELSE)) {
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
    if (expression()) {
        if (relational_operator()) {
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
    if (match(IDENTIFIER)) {
        if (match(LEFT_ROUND_BRACKET)) {
            if (variable()) {
                if (match(RIGHT_ROUND_BRACKET)) {
                    return true;
                } else {
                    put_error(MISSING_ROUND_BRACKET_ERROR);
                    goto bad;
                }
            } else if (match(RIGHT_ROUND_BRACKET)) {
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