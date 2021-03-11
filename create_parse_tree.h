/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

/*Header Guard*/
#ifndef CREATE_PARSE_TREE_H
#define CREATE_PARSE_TREE_H

#include "grammar.h"
#include "lexer.h"

/*Structure for storing a parse tree node*/
typedef struct ParseTreeNode{
    SymbolType type;                    //terminal/nonterminal ID
    int tag;                            //represent the type of symbol in a tree
    int depth;
    TokenDetails *tkn;                  // stores token corresponding to the terminal (leaf nodes)
    struct ParseTreeNode *parent;       //pointer to the parent SymbolNode of the current SymbolNode. NULL if the current node is root node
    struct ParseTreeNode *startChild;   //pointer to the first child of the current SymbolNode. NULL if current node is leaf node
    struct ParseTreeNode *rightSibling; //pointer to the next right sibling of the current SymbolNode. NULL if it's the last child or if it's the only child of the parent node
    int rule_no;                        //stores the rule no. by which current nonterminal is expanded.
}ParseTreeNode;

/*Common variables*/
extern int num_of_nodes;                //defined in common.c
extern TokenDetails* nextToken;
extern char* terminalMap[];
extern char* nonTerminalMap[];

ParseTreeNode *createParseTreeNode(SymbolType type, int tag);
ParseTreeNode* freeTreeWithRoot(ParseTreeNode* t);
ParseTreeNode* freeTreeWithoutRoot(ParseTreeNode* t);
ParseTreeNode* addNodeToTree(ParseTreeNode* t, ParseTreeNode* child);
ParseTreeNode *createParseTree(ParseTreeNode *t, Grammar *G, int d);
void internalPrintParseTree(ParseTreeNode *t, int depth);
void printParseTree(ParseTreeNode *t);

#endif