#ifndef DRIVER_H
#define DRIVER_H

#include <stdio.h>

FILE* yyerr;

void scan_error(int type);
void write_token(int symbol);


#endif  // DRIVER_H