#include "driver.hpp"

// FILE* in = nullptr;
// FILE* out = nullptr;
// FILE* err = nullptr;

int main(int argc, char* argv[]) {
    --argc, ++argv;  // Remove program name

    bool parser_flag = false;

    if (argc == 0)
        core::in = stdin;
    else if (argc == 1)
        core::in = fopen(argv[0], "r");
    else if (argc == 2) {
        for (int i = 0; i < argc; i++) {
            std::string temp = argv[i];
            if ("-p" == temp)
                parser_flag = true;
            else
                core::in = fopen(argv[i], "r");
        }
    } else {
        printf("too many parameter\n");
        return 0;
    }

    core::out = fopen("out.dyd", "w");

    core::err = fopen("out.err", "w");  // Clean out.err
    core::err = fopen("out.err", "a");

    lexer::scan();
    if (parser_flag)
        parser::parser();

    fclose(core::in);
    fclose(core::out);
    fclose(core::err);

    return 0;
}