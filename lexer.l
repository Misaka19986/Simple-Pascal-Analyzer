%top{
    #include "driver.h"
}

%option debug nodefault noyywrap

%{
    #define BEGIN_ 1    // (Avoid conflict with lex.yy.c)
    #define END 2
    #define INTEGER 3
    #define IF 4
    #define THEN 5
    #define ELSE 6
    #define FUNCTION 7
    #define READ 8
    #define WRITE 9
    #define IDENTIFIER 10
    #define CONST_NUM 11
    #define EQUAL 12    // =
    #define NOT_EQUAL 13    // <>
    #define LESS_EQUAL 14    // <=
    #define LESS_THAN 15 // <
    #define GREATER_EQUAL 16   // >=
    #define GREATER_THAN 17  // >
    #define MINUS 18    // -
    #define TIMES 19    // *
    #define ASSIGN 20 // :=
    #define LEFT_ROUND_BRACKET 21   // (
    #define RIGHT_ROUND_BRACKET 22  // )
    #define SEMICOLON 23    // ;
    #define EOLN 24 // End of line
    #define EOF_ 25  // (Avoid conflict with lex.yy.c) End of file 
    
    extern int lines = 1;
%}

id [a-zA-Z_][a-zA-Z0-9_]*
const_num (0|[1-9][0-9]*|-[1-9][0-9]*)

%%
"begin" {write_token(BEGIN_);}
"end"   {write_token(END);}
"integer"   {write_token(INTEGER);}
"if"    {write_token(IF);}
"then"  {write_token(THEN);}
"else"  {write_token(ELSE);}
"function"  {write_token(FUNCTION);}
"read"  {write_token(READ);}
"write" {write_token(WRITE);}

{id}    {
            if(yyleng > 16)
                scan_error(1);
            else
                write_token(IDENTIFIER);
        }

{const_num} {
                if(yyleng > 16)
                    scan_error(1);
                else
                    write_token(CONST_NUM);
            }

"=" {write_token(EQUAL);}
"<" {write_token(LESS_THAN);}
">" {write_token(GREATER_THAN);}
"-" {write_token(MINUS);}
"*" {write_token(TIMES);}
"(" {write_token(LEFT_ROUND_BRACKET);}
")" {write_token(RIGHT_ROUND_BRACKET);}
";" {write_token(SEMICOLON);}
":="    {write_token(ASSIGN);}
"<="    {write_token(LESS_EQUAL);}
">="    {write_token(GREATER_EQUAL);}
"<>"    {write_token(NOT_EQUAL);}

[ \t\r]

\n  {
        write_token(EOLN);
        lines++;
    }

.   {scan_error(0);}

<<EOF>> {write_token(EOF_);yyterminate();}

%%

int main(int argc, char* argv[]){
    ++argv, --argc; // Skip program name
    if(argc > 0)
        yyin = fopen(argv[0], "r");
    else
        yyin = stdin;
    yyout = fopen("out.dyd", "w");

    yyerr = fopen("out.err", "w");  // Clean out.err
    yyerr = fopen("out.err", "a");

    yylex();

    fclose(yyout);
    fclose(yyerr);
}

void scan_error(int type){
    switch(type){
        case 0:
            fprintf(yyerr, "syntax error: '%s', at line %d\n", yytext, lines);
            break;
        case 1:
            fprintf(yyerr, "identifier or constant is too long: '%s', at line %d, which is %d\n", yytext, lines, yyleng);
            break;
        default:
            break;
    }
}

void write_token(int symbol){
    if(symbol == 24) yytext = "EOLN";
    else if(symbol == 25) yytext = "EOF";
    fprintf(yyout, "%16s %d\n", yytext, symbol);
}