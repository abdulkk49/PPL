/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

/*Header Guard*/
#ifndef COMMON_H
#define COMMON_H

/*Color codes for printing to terminal*/
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

/*General Constructs*/
#define NO_OF_GRAMMAR_RULES 59
#define NO_OF_TERMINALS 32
#define NO_OF_NONTERMINALS 31
#define MAX_ALLOWED_DECLARATIONS 21
#define MAX_ALLOWED_DIMENSION 21
#define sc startChild
#define rs rightSibling
#define HASH_DIM 35

/*Standard headers*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

/*Important Definitions for all files*/
/*Self-Explanatory*/
typedef enum NonTerminal {
    programStart,
    declaration,
    declarations,
    assignment,
    assignments,
    primDatatype,
    IDList,
    arrDatatype,
    jarrDatatype,
    extendIDList,
    IDNum,
    jaggedInit,
    jaggedValues,
    numList,
    leftHandSide,
    lvalueID,
    lvalueArr,
    expression,
    indices,
    arithmeticExpr,
    opPlusMinus,
    term,
    factor,
    opMuLDiv,
    option3D,
    booleanExpr,
    extendArithmeticExpr,
    extendTerm,
    logicalOp,
    commonList,
    arrayDims
}NonTerminal;

typedef enum Terminal{ 
    AND,
    ARRAY,
    ASSIGNOP,
    BOOLEAN,
    COLON,
    CYBC,
    CYBO,
    DECLARE,
    DIV,
    EPSILON,
    ELLIPSIS,
    ID,
    INTEGER,
    JAGGED,
    LIST,
    MINUS,
    MUL,
    NUM,
    OF,
    OR,
    RBO,
    RBC,
    PLUS,
    PROGRAM,
    R1,
    REAL,
    SEMICOLON,
    SIZE,
    SQBC,
    SQBO,
    VALUES,
    VARIABLES
}Terminal;

/*Common variables*/
extern char* terminalMap[];         //defined in common.c
extern char* nonTerminalMap[];
extern int num_of_nodes;

/*Common function declarations*/
int findNonTerminal(char *str);
int findTerminal(char *str);













// typedef enum PrimitiveType{
//     integer,
//     real,
//     boolean,
// }PrimitiveType;

//  typedef struct rectangularArrayType{
//     PrimitiveType basicElementType;
//     int dimensions;
//     char** range1;
//     char** range2;
// }rectangularArrayType;

// typedef struct jaggedArrayType{
//     int dimensions;
//     int range1[2];
//     int *range2_2D;
//     int **range2_3D;
//     PrimitiveType basicElementType;
// }jaggedArrayType;

// typedef union typeExpression{
//     PrimitiveType primitive;
//     rectangularArrayType rectArray;
//     jaggedArrayType jaggedArray;
// }typeExpression;

// typedef struct tableEntry{
//     int type;     //0 -- primitive; 1 -- rectangular; 2 -- jagged
//     char name[21];
//     char arrayType[17]; // -- Static ; -- Dynamic ; -- Not Applicable
//     typeExpression typeExpr;
//     struct tableEntry *next;
// }tableEntry;

#endif
