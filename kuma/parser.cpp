#include "driver.hpp"

/*
 * token[0]: the token name
 * token[1]: the type number
 * token[2]: token location
 */
std::vector<std::string> token;

/*
 * to mark the scope of a variable;
 * when match a begin, it plus 1 (entering the scope)
 * when match a end, it minus 1 (leaving the scope)
 */
int level = 0;

/*
 * sometimes declaration or execution can be an empty statement
 * but next parser cant check that only through tokens, so we need
 * a new variable
 */
/*
 * and sometime the declaration/execution are not blank
 * but they are illegal
 */
bool is_declaration_illegal = false;
bool is_declaration_blank = false;
bool is_execution_illegal = false;
bool is_execution_blank = false;

/*
 * matching integer identifier doesn't mean that
 * it is a variable declaration, it might be a wrong function
 * declaration.
 *
 * if we match variable declaration, we need to parse next
 * token to check if it is a wrong function declaration,
 * which breaks the statement scope.
 *
 * so, we need a bool to mark this situation
 */
bool is_checking_function = false;

/*
 * when generating a variable, it needs to know
 * which proc it is in
 */

int current_function_index = 0;

/*
 * when on condition, execution doesn't need a ';'
 * which will course a disruption
 */
// bool on_condition = false;

bool parser::parser() {
    printf("\nparser begin\n");
    fprintf(core::err, "--parser\n");

    if (program()) {
        print_all_table();
        return true;
    } else {
        print_all_table();
        return false;
    }
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
        if (symbol == BEGIN) {
            level++;
            current_function_index++;
        } else if (symbol == END) {
            level--;
            current_function_index--;
        }
        return true;
    } else
        return false;
}

void parser::put_error(int type) {
    printf("putting an error :%d\n\n", type);

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
            fprintf(core::err, "missing 'function': at line %s\n",
                    token[2].c_str());
            break;
        case MISSING_ROUND_BRACKET_ERROR:
            fprintf(core::err, "missing '(' or ')': getting '%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case MISSING_ELSE_ERROR:
            fprintf(core::err, "missing 'else': getting '%s', at line %s\n",
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
        case UNDECLARED_VARIABLE_ERROR:
            fprintf(core::err, "undeclared variable:'%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case UNDECLARED_FUNCTION_ERROR:
            fprintf(core::err, "undeclared function:'%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case DUPLICATED_VARIABLE_DECLARATION:
            fprintf(core::err, "variable has been declared:'%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
        case DUPLICATED_FUNCTION_DECLARATION:
            fprintf(core::err, "function has been declared:'%s', at line %s\n",
                    token[0].c_str(), token[2].c_str());
            break;
    }
}

int put_variable(std::string name, bool kind) {
    struct variable_table_type temp;
    temp.name = name;
    temp.proc = function_table[current_function_index].name;
    temp.is_parameter = kind;
    temp.type = "integer";
    temp.level = level;
    temp.index = variable_table.size();

    if (!check_variable(name)) {
        variable_table.push_back(temp);
        return 1;
    } else {
        return DUPLICATED_VARIABLE_DECLARATION;
    }
}

bool check_variable(std::string name) {
    for (int i = 0; i < variable_table.size(); i++) {
        struct variable_table_type temp = variable_table[i];
        if (name == temp.name) {
            if (function_table[current_function_index].name == temp.proc &&
                level == temp.level) {
                // current function has declared this variable
                return true;
            }
        }
    }

    return false;
}

int put_function(std::string name) {
    struct function_table_type temp;
    temp.name = name;
    if (function_table.size() == 0) {
        temp.proc = "";
    } else {
        temp.proc = function_table[current_function_index].name;
    }
    temp.type = "integer";
    temp.level = level;
    temp.first_index = 0;
    temp.last_index = 0;

    if (!check_function(name)) {
        function_table.push_back(temp);
        return 1;
    } else {
        return DUPLICATED_FUNCTION_DECLARATION;
    }
}
bool check_function(std::string name) {
    for (int i = 0; i < function_table.size(); i++) {
        struct function_table_type temp = function_table[i];
        if (name == temp.name) {
            if (function_table[current_function_index].name == temp.proc &&
                level == temp.level) {
                // current function has declared this variable
                return true;
            }
        }
    }

    return false;
}

void print_all_table() {
    fprintf(core::var, "name\tproc\tis_parameter\ttype\tlevel\tindex\n");
    for (int i = 0; i < variable_table.size(); i++) {
        struct variable_table_type temp = variable_table[i];
        fprintf(core::var, "%s\t%s\t%d\t%s\t%d\t%d\n", temp.name.c_str(),
                temp.proc.c_str(), temp.is_parameter, temp.type.c_str(),
                temp.level, temp.index);
    }

    fprintf(core::pro, "name\tproc\ttype\tlevel\tfirst_index\tlast_index\n");
    for (int i = 0; i < function_table.size(); i++) {
        struct function_table_type temp = function_table[i];
        fprintf(core::pro, "%s\t%s\t%s\t%d\t%d\t%d\n", temp.name.c_str(),
                temp.proc.c_str(), temp.type.c_str(), temp.level,
                temp.first_index, temp.last_index);
    }
}

bool parser::program() {
    printf("program\n\n");

    parse_next_token();
    if (sub_program()) {
        return true;
    } else {
        return false;
    }
}

bool parser::sub_program() {
    printf("sub_program\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(BEGIN)) {
        put_function("main");
        parse_next_token();
        if (declaration_table()) {
            // to check declaration is ended, we have move to next token
            // behind
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
        if (is_declaration_blank || is_checking_function) {
            // declaration is blank
            // then token stops at ';' or 'end' or something else
            // no need to parse next token
            // remain is_declaration_blank true
            is_checking_function = false;
        } else {
            // declaration is not blank and legal
            // so go next token to check if it is a loop
            parse_next_token();
        }
        if (declaration_table_prime()) {
            is_declaration_blank = false;
            is_declaration_illegal = false;
            return true;
        } else {
            goto bad;
        }
    } else {
        put_error(BAD_DECLARATION_ERROR);
        goto bad;
    }

bad:
    is_declaration_blank = false;
    is_declaration_illegal = false;
    return false;
}

bool parser::declaration_table_prime() {
    printf("declaration_table_prime\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(SEMICOLON)) {
        parse_next_token();
        if (declaration()) {
            if (is_declaration_blank || is_checking_function) {
                is_checking_function = false;
            } else {
                parse_next_token();
            }
            if (declaration_table_prime()) {
                is_declaration_blank = false;
                is_declaration_illegal = false;
                return true;
            } else {
                goto bad;
            }
        } else if (is_declaration_illegal) {
            put_error(BAD_DECLARATION_ERROR);
            is_declaration_illegal = false;
            return false;
        }
        // matching the ';' or nothing already prove legal
        // if there is something behind the ';' and it's not declaration
        // then we have left the scope, leave the token to next checker
    } else if (is_declaration_blank) {
        // declaration is blank, and no more declaration
        is_declaration_blank = false;
        return true;
    } else {
        // declaration is not blank, but it doesn't end with ';'
        put_error(MISSING_SEMICOLON_ERROR);
        goto bad;
    }

bad:
    is_declaration_blank = false;
    is_declaration_illegal = false;
    return false;
}

bool parser::declaration() {
    printf("declaration\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(INTEGER)) {
        is_declaration_blank = false;
        is_declaration_illegal = false;
        parse_next_token();
        // variable_declaration and function_declaration
        // both begin with integer
        if (variable_declaration()) {
            is_checking_function = true;
            // check if it is a function declaration
            parse_next_token();
            if (match(LEFT_ROUND_BRACKET)) {
                is_declaration_illegal = true;
                put_error(MISSING_FUNCTION_ERROR);
                goto bad;
            } else if (match(IDENTIFIER)) {
                is_declaration_illegal = true;
                goto bad;
            }

            return true;
        } else if (function_declaration()) {
            return true;
        } else {
            // illegal declaration
            is_declaration_illegal = true;
            goto bad;
        }
    } else {
        // match nothing
        is_declaration_blank = true;
        return true;
    }

bad:
    return false;
}

bool parser::variable_declaration() {
    printf("variable_declaration\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (variable()) {
        // parse_next_token();
        // if(match(LEFT_ROUND_BRACKET)){
        //     // it might be a function
        // }
        return true;
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
        put_error(MISSING_IDENTIFIER_ERROR);
        return false;
    }

bad:
    is_declaration_illegal = true;
    return false;
}

bool parser::parameter() {
    printf("parameter\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(INTEGER)) {
        parse_next_token();
        if (variable()) {
            return true;
        } else {
            goto bad;
        }
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
        // execution need to look forward to check
        // if the loop is ended
        // so execution() makes the decisions about
        // how to parse next token
        if (execution_table_prime()) {
            is_execution_blank = false;
            is_execution_illegal = false;
            return true;
        } else {
            goto bad;
        }
    } else {
        put_error(BAD_EXECUTION_ERROR);
        goto bad;
    }

bad:
    is_execution_blank = false;
    is_declaration_illegal = false;
    return false;
}

bool parser::execution_table_prime() {
    printf("execution_table_prime\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (match(SEMICOLON)) {
        parse_next_token();
        if (execution()) {
            // execution need to look forward to check
            // if the loop is ended
            // so execution() makes the decisions about
            // how to parse next token
            if (execution_table_prime()) {
                is_execution_blank = false;
                is_execution_illegal = false;
                return true;
            } else {
                goto bad;
            }
        } else if (is_execution_illegal) {
            put_error(BAD_EXECUTION_ERROR);
            is_execution_illegal = false;
            return false;
        }
        // matching the ';' already prove legal
        // if there is something behind the ';' and it's not execution
        // then we have left the scope, leave the token to next checker
    } else if (is_execution_blank) {
        // execution is blank, and no more execution
        is_execution_blank = false;
        return true;
        // } else if (on_condition && match(END)){
        //     on_condition = false;
        //     return true;
    } else {
        // execution is not blank, but it doesn't end with ';'
        put_error(MISSING_SEMICOLON_ERROR);
        goto bad;
    }

bad:
    is_execution_blank = false;
    is_execution_illegal = false;
    return false;
}

bool parser::execution() {
    printf("execution\n");
    printf("parsing '%s'\n\n", token[0].c_str());

    if (read_statement()) {
        is_execution_blank = false;
        parse_next_token();
        return true;
    } else if (write_statement()) {
        is_execution_blank = false;
        parse_next_token();
        return true;
    } else if (assignment()) {
        is_execution_blank = false;
        // assignment need to look forward to check
        // if it is ended
        return true;
    } else if (condition()) {
        is_execution_blank = false;
        return true;
    } else if (!is_execution_illegal) {
        // no match and no illegal
        // so execution is blank
        is_execution_blank = true;
        return true;
    } else {
        // illegal
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
            if (variable()) {
                parse_next_token();
                if (match(RIGHT_ROUND_BRACKET)) {
                    return true;
                } else {
                    put_error(MISSING_ROUND_BRACKET_ERROR);
                    is_execution_illegal = true;
                    goto bad;
                }
            } else {
                put_error(MISSING_IDENTIFIER_ERROR);
                is_execution_illegal = true;
                goto bad;
            }
        } else {
            put_error(MISSING_ROUND_BRACKET_ERROR);
            is_execution_illegal = true;
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
            if (variable()) {
                parse_next_token();
                if (match(RIGHT_ROUND_BRACKET)) {
                    return true;
                } else {
                    put_error(MISSING_ROUND_BRACKET_ERROR);
                    is_execution_illegal = true;
                    goto bad;
                }
            } else {
                put_error(MISSING_IDENTIFIER_ERROR);
                is_execution_illegal = true;
                goto bad;
            }
        } else {
            put_error(MISSING_ROUND_BRACKET_ERROR);
            is_execution_illegal = true;
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
                is_execution_illegal = true;
                goto bad;
            }
        } else {
            is_execution_illegal = true;
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
            // expression is a combinations of items
            // even it is followed by a illegal token
            // the scanned part is legal
            // leave the illegal part to upper class
            return true;
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
                // expression is a combinations of items
                // even it is followed by a illegal token
                // the scanned part is legal
                // leave the illegal part to upper class
                return true;
            }
        } else {
            goto bad;
        }
    } else if (match(SEMICOLON)) {
        // no more item
        return true;
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
        if (item_prime()) {
            return true;
        } else {
            // item is a combinations of factors
            // even it is followed by a illegal token
            // the scanned part is legal
            // leave the illegal part to upper class
            return true;
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
        // it may be a function_call
        parse_next_token();
        if (match(LEFT_ROUND_BRACKET)) {
            if (function_call()) {
                parse_next_token();
                return true;
            } else {
                goto bad;
            }
        } else {
            // not a function call
            // return anyway
            return true;
        }
    } else if (match(CONST_NUM)) {
        parse_next_token();
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
            if (match(THEN)) {
                parse_next_token();
                if (execution()) {
                    if (match(ELSE)) {
                        parse_next_token();
                        if (execution()) {
                            return true;
                        } else {
                            is_execution_illegal = true;
                            goto bad;
                        }
                    } else {
                        put_error(MISSING_ELSE_ERROR);
                        is_execution_illegal = true;
                        goto bad;
                    }
                } else {
                    is_execution_illegal = true;
                    goto bad;
                }
            } else {
                is_execution_illegal = true;
                goto bad;
            }
        } else {
            is_execution_illegal = true;
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

    if (match(LEFT_ROUND_BRACKET)) {
        parse_next_token();
        if (expression()) {
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
        goto bad;
    }

bad:
    return false;
}