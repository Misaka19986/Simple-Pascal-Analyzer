#include "driver.hpp"

// FILE* in = nullptr;
// FILE* out = nullptr;
// FILE* err = nullptr;

int main(int argc, char* argv[]) {
    --argc, ++argv;  // Remove program name

    bool parser_flag = false;

    /* release */
    if (argc == 0)
        core::in = stdin;  // no parameter
    else if (argc == 1) {
        if (0 == strcmp(argv[0], "-p")) {  // 1 parameter
            core::in = stdin;
            parser_flag = true;
        } else
            core::in = fopen(argv[0], "r");
    } else if (argc == 2) {
        for (int i = 0; i < argc; i++) {  // 2 parameter
            std::string temp = argv[i];
            if ("-p" == temp)
                parser_flag = true;
            else {
                core::in = fopen(argv[i], "r");
            }
        }
    } else {
        printf("too many parameter\n");
        return 0;
    }

    /* debug */
    // core::in = fopen("../test/test1.pas", "r");
    // parser_flag = true;

    core::out = fopen("middle.dyd", "w");

    core::err = fopen("middle.err", "w");  // Clean middle.err
    core::err = fopen("middle.err", "a");

    core::var = fopen("middle.var", "w");
    core::pro = fopen("middle.pro", "w");

    if (lexer::scan()) {
        if (parser_flag) {
            fclose(core::in);
            core::in = fopen("middle.dyd", "r");
            fclose(core::out);
            core::out = fopen("middle.dys", "w");

            if (parser::parser())
                printf("parse success\n");
            else
                printf("parse failed\n");
        }

        printf("compile done\n");
    } else {
        printf("lexical analysis failed\n");
    }

    fclose(core::in);
    fclose(core::out);
    fclose(core::err);
    fclose(core::var);
    fclose(core::pro);

    return 0;
}