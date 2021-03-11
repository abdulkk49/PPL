/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

/*Header Guard*/
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include "common.h"

/*Union to denote the type of a symbol - terminal or nonterminal*/
typedef union{
    Terminal terminal;
    NonTerminal nonterminal;
}SymbolType;

/*Tagged union for denoting a symbol*/
typedef struct SymbolNode{
    SymbolType type;               //denotes type of the symbol (either non-terminal or terminal) -> usage of tagged union
    int tag;                       //denotes tag for the tagged union representation of SymbolType. 0 -> Terminal, 1 -> NonTerminal
    struct SymbolNode *next;       //next pointer to next SymbolNode in a linkedlist
}SymbolNode;

/*Structure for denoting linked list of SymbolNodes
used to represent list of terminals/non-terminals occuring on rhs of a grammar rule)*/
typedef struct SymbolNodesList{
    SymbolNode *start;      //head of the linkedlist of SymbolNodes
    int len;                //length of the linkedlist
}SymbolNodesList;

/*Structure for denoting a grammar rule*/
typedef struct Rule{
    NonTerminal lhs;        //non-terminal appearing on lhs of the production rule
    SymbolNodesList rhs;    //linked list of symbols
}Rule;

/*Array of Grammar Rules*/
typedef struct Grammar{
    Rule *rules;            //array of rule pointers. eg:- rules[0] would point to the first rule; rules[94] would denote the last rule(95th rule)
    int no_of_rules;        //number of rules (eg:- consider S -> Ab | BC | Def as 3 rules) 
}Grammar;

/*Common variables*/
extern char *terminalMap[];         //defined in common.c
extern char* nonTerminalMap[];

/*Grammar reading functions*/
Grammar* initializeGrammar();
void populateRHS(Rule *rl, char *buff, int i);
int populateGrammarWithRules(char *grammarFile, Grammar* G);

/*Grammar debug functions*/
void printGrammarRules(Grammar *G);

#endif
