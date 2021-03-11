/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/
#include "driver.h"

/*Driver Code*/
int main(int argc, char* argv[]) {
    printf(PURPLE"\n--------------------PHASE I : GRAMMAR READING AND GENERATION OF GRAMMAR RULES ARRAY--------------------\n"RESET);
    Grammar *G = initializeGrammar();
    populateGrammarWithRules("Grammar.txt", G);
    printGrammarRules(G);

    printf(PURPLE"\n--------------------PHASE II : LEXICAL ANALYSIS : GENERATION OF TOKENS--------------------\n"RESET);

    tokenStream s;
    s.start = NULL;
    s.end = NULL;
    TokenDetails* tk = tokeniseSourceCode(argv[1], &s);
    if(tk != NULL){
        printf(RED"%s at line number %d\n"RESET, tk->error, tk->line_no);
        exit(0);
    } else{
        printf(GREEN"\nTOKENS GENERATED SUCCESSFULLY FOR SOURCE FILE : %s\n"RESET, argv[1]);
        printTokens(&s);
    }

    printf(PURPLE"\n--------------------CREATION OF A PARSE TREE--------------------\n"RESET);

    ParseTreeNode *pt;
    SymbolType parent;
    parent.nonterminal = findNonTerminal("programStart");
    pt = createParseTreeNode(parent, 1);
    pt->rule_no = 1;
    nextToken = s.start;
    if(pt->sc == NULL)
        pt = createParseTree(pt, G, 0);

    printf(GREEN"\nPARSE TREE CREATED SUCCESSFULLY FOR SOURCE FILE : %s\n"RESET, argv[1]);

    printf(RED"\n-------PREORDER TRAVERSAL OF A PARSETREE---------\n"RESET);
    printParseTree(pt);
    // printf("\n"RED"Declaration Errors:\n");
   
    // // FILE *fp = fopen("tree.txt", "a");
    // tableEntry** tet = initialiseTable();
    // int choice = 1;
    // while(1) {
    //     printf("\n\n0. Exit\n");
    //     printf("1. Create parse tree\n");
    //     printf("2. Traverse the parse tree and detect declaration errors\n");
    //     printf("3. Print the parse tree\n");
    //     printf("4. Print Type Expression Table (Invalid identifiers not included)\n");
    //     printf("Enter choice: ");
    //     scanf("%d", &choice);
    //     printf("\n");
    //     if(choice == 1) {
    //         if(pt->sc == NULL)
    //             pt = createParseTree(pt, G);
    //         if(pt == NULL){
    //             printf("Invalid syntax..\n");
    //             exit(0);
    //         }
    //     }
    //     else if(choice == 2) {
    //         if(pt->sc == NULL) pt = createParseTree(pt, G);
    //         printf("\n"RED"Declaration Errors:\n");
    //         traverseParseTree(pt, tet);
    //         printf("\nTypeExpressionTable was constructed.\n");
    //     } 
    //     else if(choice == 3) {
    //         if(pt->sc == NULL) pt = createParseTree(pt, G);
    //         printf("\nPreorder traversal of parse tree..\n");
    //         printParseTree(pt);
    //     } 
    //     else if(choice == 4){
    //         if(pt->sc == NULL) { // create first
    //             pt = createParseTree(pt, G);
    //             traverseParseTree(pt, tet);
    //             printf("\nTypeExpressionTable was constructed.\n");
    //         } 
    //         else if(pt->sc != NULL && tet == NULL) {
    //             traverseParseTree(pt, tet);
    //             printf("\nTypeExpressionTable was constructed.\n");
    //         }
    //         printf("Printing Type Expression table..\n");
    //         printTypeExpressionTable(tet);
    //     }
    //     else if(choice == 0){
    //         exit(0);
    //     }
    //     else {
    //         printf("Incorrect choice, try again!\n");
    //     }
    // }
}
