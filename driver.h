/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/
/*Header Guard*/
#ifndef DRIVER_H
#define DRIVER_H

#include "common.h"
#include "grammar.h"
#include "lexer.h"
#include "create_parse_tree.h"

extern TokenDetails* nextToken;     //defined in common.c
extern char* terminalMap[];         //defined in common.c
extern char* nonTerminalMap[];

/*Driver declaration*/
int main(int argc, char* argv[]);

#endif