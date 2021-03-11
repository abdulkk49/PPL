/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

#include "common.h"
#include "lexer.h"

/*Defining common variables*/
char *terminalMap[] ={ 
    "AND",
    "ARRAY",
    "ASSIGNOP",
    "BOOLEAN",
    "COLON",
    "CYBC",
    "CYBO",
    "DECLARE",
    "DIV",
    "#",
    "ELLIPSIS",
    "ID",
    "INTEGER",
    "JAGGED",
    "LIST",
    "MINUS",
    "MUL",
    "NUM",
    "OF",
    "OR",
    "RBO",
    "RBC",
    "PLUS",
    "PROGRAM",
    "R1",
    "REAL",
    "SEMICOLON",
    "SIZE",
    "SQBC",
    "SQBO",
    "VALUES",
    "VARIABLES"
};

char *nonTerminalMap [] ={
	"programStart",
    "declaration",
    "declarations",
    "assignment",
    "assignments",
    "primDatatype",
    "IDList",
    "arrDatatype",
    "jarrDatatype",
    "extendIDList",
    "IDNum",
    "jaggedInit",
    "jaggedValues",
    "numList",
    "leftHandSide",
    "lvalueID",
    "lvalueArr",
    "expression",
    "indices",
    "arithmeticExpr",
    "opPlusMinus",
    "term",
    "factor",
    "opMuLDiv",
    "option3D",
    "booleanExpr",
    "extendArithmeticExpr",
    "extendTerm",
    "logicalOp",
    "commonList",
    "arrayDims"
};

TokenDetails* nextToken = NULL;         //used in create_parse_tree.c
int num_of_nodes = 0;                   //used in create_parse_tree.c

/*This function returns the index of non-terminal from the nonTerminalMap array
    Parameters :
        s : non terminal string
    Returns :
        i : index in the enum/map
*/
int findNonTerminal(char *str){
    int i;
    for(i = 0; i < NO_OF_NONTERMINALS; i++){
        if(strcmp(str, nonTerminalMap[i]) == 0)
            return i;    
    }
	return -1;
}

/*This function returns the index of terminal from the terminalMap array  
    Parameters :
        s : terminal string
    Returns :
        i : index in the enum/map
*/
int findTerminal(char *str){
    int i;
    for(i = 0; i < NO_OF_TERMINALS; i++)
    {
        if(strcmp(str, terminalMap[i]) == 0)
            return i;    
    }
	return -1;
}


