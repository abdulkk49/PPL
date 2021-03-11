/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

/*Header Guard*/
#ifndef HASHMAP_H
#define HASHMAP_H

#include "common.h"
#include "traverse_parse_tree.h"

/*Common variables*/
extern char* terminalMap[];
extern char* nonTerminalMap[];

/*Hashmap helper functions declarations*/
int hashFunction(char *arr);
tableEntry** initialiseTable();
void addtoTable(tableEntry* t, tableEntry **T);
tableEntry* lookup(char *identifier, tableEntry **T);
tableEntry* createTableEntry(int type, char *name, char *arrayType);


#endif