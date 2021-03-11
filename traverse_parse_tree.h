/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

/*Header Guard*/
#ifndef TRAVERSE_PARSE_TREE_H
#define TRAVERSE_PARSE_TREE_H

#include "common.h"
#include "create_parse_tree.h"

/*Enum for primitive data types*/
typedef enum PrimitiveType{
    integer,
    real,
    boolean,
}PrimitiveType;

/*Structure to store rectangular array details*/
typedef struct rectangularArrayType{
    PrimitiveType basicElementType;
    int dimensions;
    char** range1;
    char** range2;
}rectangularArrayType;

/*Structure to store jagged array details*/
typedef struct jaggedArrayType{
    int dimensions;
    int range1[2];
    int *range2_2D;
    int **range2_3D;
    PrimitiveType basicElementType;
}jaggedArrayType;

/*Union for type expression details*/
typedef union typeExpression{
    PrimitiveType primitive;
    rectangularArrayType rectArray;
    jaggedArrayType jaggedArray;
}typeExpression;

/*Tagged union for type expression table entry*/
/*Type Expression table is implemented as a hash table*/
typedef struct tableEntry{
    int type;                           //0 -- primitive; 1 -- rectangular; 2 -- jagged
    char name[21];
    char arrayType[17];                 // -- Static ; -- Dynamic ; -- Not Applicable
    typeExpression typeExpr;
    struct tableEntry *next;
}tableEntry;

/*Common variables*/
extern char *terminalMap[];             //defined in common.c
extern char* nonTerminalMap[];


char* getDataType(ParseTreeNode* nodeCheck);
ParseTreeNode* shiftRight(ParseTreeNode* node, int shift);
void findArrayDims(ParseTreeNode* node, bool* hasDynamic, int* numberOfDimensions, int i, char r1[21][21], char r2[21][21]);
void shallowCopy(tableEntry* t, tableEntry* similarEntry);
bool check2DRecursive(ParseTreeNode* node, int *j);
bool check3DRecursive(ParseTreeNode* node, int *j, int* R3);
bool assignTypeExpr(tableEntry* t, ParseTreeNode* node, char* dt, tableEntry** T);
bool addDeclaration(ParseTreeNode* declPointer, tableEntry** T);
void addDeclarationsToTable(ParseTreeNode* declPointer, tableEntry** T);
// void printError(ParseTreeNode* node, Terminal op, tableEntry* l, tableEntry* r, char* msg);
// Terminal getOp(ParseTreeNode* node);
// tableEntry* checkArrayCompatibility(tableEntry *l, tableEntry *r, ParseTreeNode* node, Terminal term, tableEntry **T);
// void checkArrayBounds(ParseTreeNode* node, tableEntry *t);
// tableEntry* commonCheck(tableEntry *l, tableEntry *r, ParseTreeNode* nodeCheck, Terminal t, tableEntry **T);
// tableEntry* getTypeOfFactor(ParseTreeNode* node, tableEntry **T);
// tableEntry* getTypeOfExtendTerm(ParseTreeNode* node, tableEntry **T);
// tableEntry* getTypeOfTerm(ParseTreeNode* node, tableEntry** T);
// tableEntry* getTypeOfExtendArithExpr(ParseTreeNode* node, tableEntry **T);
// tableEntry* getTypeOfBoolean(ParseTreeNode* node, tableEntry **T);
// tableEntry* getTypeOfExpression(ParseTreeNode* node, tableEntry **T);
// void checkAssignment(ParseTreeNode* assignPointer, tableEntry **T);
// void checkAssignmentsFromTable(ParseTreeNode* assignPointer, tableEntry** T);

void traverseParseTree(ParseTreeNode* root, tableEntry** T);
void printDebug(ParseTreeNode* t);
void printTypeExpression(typeExpression typeExpr, int type);
void printTypeExpressionTable(tableEntry** tet);

#endif

