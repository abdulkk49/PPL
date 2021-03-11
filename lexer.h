/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

/*Header Guard*/
#ifndef LEXER_H
#define LEXER_H

#include "common.h"

/*Structure to store each token*/
typedef struct TokenDetails{     //Structure of a Token
    bool is_num;                 //true if token is NUM, false if not
    bool is_error;               //true if lexical error, false if not. Default value is false
    char token[21];              //stores the token of the corresponding lexeme
    char lexeme[21];             //stores the lexeme
    Terminal terminal_no;        
    int line_no;                 //stores the line number in which the token is found
    int value;                   //stores the value of NUM tokens 
    char *error;                 //stores error message in case of lexical error. NULL if no error
    struct TokenDetails *prev;
    struct TokenDetails *next;   //next and prev for making the linked list
}TokenDetails; 

/*Linked List of Tokens*/
typedef struct tokenStream{
    TokenDetails *start, *end;
}tokenStream;

/*Common variables*/
extern char *terminalMap[];     //defined in common.c
extern char *nonTerminalMap[];

/*Tokenizer functions*/
TokenDetails* tokeniseSourceCode(char* fileName, tokenStream *s);
void addToken(TokenDetails *node, tokenStream *s);
TokenDetails* createToken(char* lexeme, int line_no);
bool isNumeric(char* str);
void printTokens(tokenStream *s);

#endif

