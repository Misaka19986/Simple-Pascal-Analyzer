#include "driver.hpp"

/**
 * @brief A main function to scan a character everytime from input file.
 * Check if the character forms a valid token, and do output.
 * 
 */
void lexer::scan(){
    std::string token = "";
    while(!feof(core::in)){
        char ch = getc(core::in);
        printf("here is scanning %c\n", ch);

        if(' ' == ch || '\n' == ch || '\t' == ch || '\r' == ch || EOF == ch){
            // There is a blank or EOF
            if('\n' == ch) ++core::lines;

            if(token.empty())
                // Do nothing
                ;
            else{
                int res = check_type(token);

                if(res >= 255)
                    // There is an error
                    put_error(token, res);
                else
                    // Token matchs a keyword
                    put_token(token, res);
                
                token.clear();  // Empty the token
            }

            if(EOF == ch) put_token("EOF", EOF_);

        } else {
            token += ch;
        }
    }

    return;
}

/**
 * @brief A function to put a formatted output into output file.
 * 
 * @param s The name of the token
 * @param symbol The number of that symbol, check driver.hpp for more
 * 
 */
void lexer::put_token(std::string s, int symbol){
    printf("putting a token %s:%d\n", s.c_str(), symbol);
}

/**
 * @brief A function to check the token type.
 * 
 * @param s The token
 * 
 * @return The corresponding number of the token, -1 as error
 * 
 */
int lexer::check_type(std::string s){
    return 255;
}

/**
 * @brief A function to put a formatted output into error file.
 * 
 * @param s The word that caused an error
 * @param type The error type
 * 
 */
void lexer::put_error(std::string s, int type){
    printf("putting an error %s:%d\n", s.c_str(), type);
    switch(type){
        case SYNTAX_ERROR:
            fprintf(core::err, "syntax error: '%s', at line %d\n", s.c_str(), type);
            break;
        case INVALID_LENGTH_ERROR:
            fprintf(core::err, "identifier or constant is too long: '%.10s'..., at line %d\n", s.c_str(), type);
            break;
    }
}
