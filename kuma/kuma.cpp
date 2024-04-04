#include "driver.hpp"

// FILE* in = nullptr;
// FILE* out = nullptr;
// FILE* err = nullptr;

int main(int argc, char* argv[]){
    --argc, ++argv; // Remove program name

    if(argc > 0)
        core::in = fopen(argv[0], "r");
    else
        core::in = stdin;

    core::out = fopen("out.dyd", "w");

    core::err = fopen("out.err", "w");  // Clean out.err
    core::err = fopen("out.err", "a");

    lexer::scan();

    fclose(core::in);
    fclose(core::out);
    fclose(core::err);
    
    return 0;
}