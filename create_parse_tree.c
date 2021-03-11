/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

#include "create_parse_tree.h"

/*This function creates a parse tree node
    Parameters:
        type : Symboltype of the current node(T/NT)
        tag : 1 for NT and 0 for T
    Returns : 
        nd : Parse tree node
*/
ParseTreeNode* createParseTreeNode(SymbolType type, int tag) {
    ParseTreeNode *nd = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    nd->parent = NULL;
    nd->sc = NULL;
    nd->rs = NULL;
    if(tag == 0)
        nd->type.terminal = type.terminal;
    else
        nd->type.nonterminal = type.nonterminal;
    nd->tkn = NULL;
    nd->tag = tag;
    nd->rule_no = -1;
    nd->depth = 0;
	num_of_nodes++;
    return nd;
}

/*This function deletes the parse tree rooted at node t
    Parameters:
        t : root of the parse tree to be deleted
    Returns : 
        NULL
*/
ParseTreeNode* freeTreeWithRoot(ParseTreeNode* t){
    // printf("Deleting..\n");
    if(t == NULL) return NULL;
    if(t->sc == NULL && t->rs == NULL){
        free(t);
        num_of_nodes--;
        return NULL;
    }
    t->rs = freeTreeWithRoot(t->rs);
    t->sc = freeTreeWithRoot(t->sc);
    free(t);
    num_of_nodes--;
    return NULL;
}

/*This function deletes the children of parse tree
rooted at node t.
    Parameters:
        t : root of the parse tree to be deleted
    Returns : 
        t : root of the parse tree
*/
ParseTreeNode* freeTreeWithoutRoot(ParseTreeNode* t){
    // printf("Deleting..\n")
    if(t == NULL) return t;
    if(t->sc == NULL && t->rs == NULL) return t;
    t->rs = freeTreeWithRoot(t->rs);
    t->sc = freeTreeWithRoot(t->sc);
    return t;
}

/*This function adds the child to parse tree
rooted at node t.
    Parameters:
        t : root of the parse tree
        child : node to be added
    Returns : 
        t : root of the parse tree
*/
ParseTreeNode* addNodeToTree(ParseTreeNode* t, ParseTreeNode* child){
    if(t->sc == NULL){ //if the tree only has a root node
        t->sc = child;
    }
    else{ //if it has one node already
        ParseTreeNode * temp = t->sc;
        while(temp->rs != NULL)
            temp = temp->rs;
        temp->rs = child;
    }
    return t;
}

/*This function recursively genreates the parse tree from Grammar
and tokens of Source Code
    Parameters:
        t : root of the parse tree
        G : Grammar pointer points to array of rules
    Returns : 
        t : root of the parse tree
*/
ParseTreeNode* createParseTree(ParseTreeNode *t, Grammar *G, int d){
    //Iterate over all rules and find the non terminal which is at the root t
    // printf("-----------------Recursive call for : %s------------------------\n", nonTerminalMap[t->type.nonterminal]);
    TokenDetails *currToken = nextToken;  //save the starting token from which the current NT rule will be expanded
                                          //This is done because there are multiple rules for same NT 
                                          //And we might find that the token unmatches somewhere in the middle
                                          //Eg. 1. A -> abcD and inputtokens = {a, b, d, ...}
                                          //    2. A -> abhD
                                          //We will first expand the rule 1 and find that it does not match in 3rd terminal
                                          //(c != h), hence we will break and go to rule 2. So our nextToken pointer needs to be 
                                          //reset to a in order to parse the second rule from start
                                          //This can also be eliminated by using another NT for common string ab
                                          //Eg: A -> XcD
                                          //    A -> XhD  -- Doing this will eliminate the need to set currTOken again and again
                                          //    X -> ab
                                          //But, here we prefer to keep them as seperate
    for(int i = 0; i < NO_OF_GRAMMAR_RULES; i++){
        //if the non terminal is found
        if(t->type.nonterminal == G->rules[i].lhs){
            nextToken = currToken; //set the nextToken as last unmatched token
            // printf("===== Resetting token to last unmatched: %s ======\n", nextToken->token);
            // printf("Try rule no : %d\n", i+1);
            //Start traversing the rhs of that rule
            SymbolNode* RuleSym = G->rules[i].rhs.start;
            ParseTreeNode* child;
            while(RuleSym != NULL){
                t->rule_no = i + 1;
                // printf("Token to be matched : %s\n", nextToken->token);
                //if the symbol on rhs is a terminal
                if(RuleSym->tag == 0){
                    //If an epsilon rule is detected, terminate the rule
                    //For this to work, epsilon rule should come after the other
                    //rules for the same non terminal
                    //Eg. 1. A -> aB  instead of 1. A -> e
                    //    2. A -> e              2. A -> aB
                    if(RuleSym->type.terminal == EPSILON){
                        child = createParseTreeNode(RuleSym->type, 0);
                        child->tkn = createToken("#", -1);
                        child->parent = t;
                        child->rule_no = i+1;
                        // printf("\nEpsilon Termination detected : Rule passed..\n");
                        // getchar();
                        //add the node to parse tree
                        t = addNodeToTree(t, child);
                        child->depth = d+1;
                        RuleSym = RuleSym -> next;
                        continue;
                    }
                    //if rhs terminal matches the current token,
                    //consume the current token
                    if(RuleSym->type.terminal == nextToken->terminal_no){
                        // printf("Token matched: %s\n", nextToken->token);
                        // getchar();
                        child = createParseTreeNode(RuleSym->type, 0);
                        child->tkn = nextToken;
                        child->parent = t;
                        child->rule_no = i+1;
                        //assign the node to parse tree
                        t = addNodeToTree(t, child);
                        child->depth = d+1;
                        nextToken = nextToken->next;
                        RuleSym = RuleSym -> next;
                    }
                    else{ //if rhs symbol is a terminal but
                          //there is a mismatch, so we look for another rule with same lhs
                        
                        //Move token pointer to when the rule was started
                        // printf("RULE FAILS : Different token detected..%s\n", terminalMap[RuleSym->type.terminal]);
                        nextToken = currToken;
                        // printf("Backtracking now and deleting the tree..\n\n");
                        //break and free the children that are misplaced
                        //do not free the root of the tree!!!
                        break;
                    }
                }
                else{ // if the rhs symbol is a non terminal
                    //if the first symbol of rhs is a non terminal
                    //only then can a tree be empty
                    //recurse for this non terminal
                    if(t->sc == NULL){
                        // printf("\nGoing into START recursion for new NT : %s -- Root : %s\n", nonTerminalMap[RuleSym->type.nonterminal],nonTerminalMap[t->type.nonterminal] )
                        t->sc = createParseTreeNode(RuleSym->type, 1);
                        t->sc->parent = t;
                        t->sc->rule_no = i+1;
                        t->sc->depth = d+1;
                        t->sc = createParseTree(t->sc, G, d+1);
                        
                        //if the wrong rule was expanded, NULL will be returned
                        //we need to look for another rule with same lhs
                        if(t->sc == NULL){
                            // printf("RULE FAILS : Wrong NT rule expanded..%d\n", i+1);
                            break;
                        }
                        
                        // printf("Start Child Recursion Comes back for NT %s -- Root : %s\n", nonTerminalMap[RuleSym->type.nonterminal], nonTerminalMap[t->type.nonterminal] );
                    }
                    else{ //if the tree has atleast one children
                        // printf("Going into Right Sibling Recursion for NT %s -- Root : %s\n", nonTerminalMap[RuleSym->type.nonterminal], nonTerminalMap[t->type.nonterminal] );
                        ParseTreeNode * temp = t->sc;
                        while(temp->rs != NULL)
                            temp = temp ->rs;
                        temp->rs = createParseTreeNode(RuleSym->type, 1);
                        temp->rs->parent = t;
                        temp->rs->rule_no = i+1;
                        temp->rs->depth = d+1;
                        temp->rs = createParseTree(temp->rs, G, d+1);
                        //if the wrong rule was expanded, NULL will be returned
                        //we need to look for another rule with same lhs
                        if(temp->rs == NULL){
                            // printf("RULE FAILS : Wrong NT rule expanded.. %d\n", i+1);
                            break;
                        }
                        // printf("Right Sibling Recursion Comes back for NT %s -- Root : %s\n", nonTerminalMap[RuleSym->type.nonterminal], nonTerminalMap[t->type.nonterminal] );
                    }
                    //if the recursive tree building for this non terminal
                    //symbol was successful, we can move to next symbol
                    RuleSym = RuleSym -> next;
                }
            }
            //if the rule is over,
            //then we have successfully expanded so we return the tree
            if(RuleSym == NULL)
                return t;
            //if incorrect, remove the links
            //this is invoked when one of the break statements comes into play
            // printf("Deleting children of non terminal root %s\n", nonTerminalMap[t->type.nonterminal]);
            t = freeTreeWithoutRoot(t);
        }
    }
    //if the rule cannot be expanded, 
    //means there is an error
    //we free the tree with its root
    // printf("Deleting non terminal root %s\n", nonTerminalMap[t->type.nonterminal]);
    t = freeTreeWithRoot(t);
    return NULL;
}

void internalPrintParseTree(ParseTreeNode *t, int depth) {
    if(t == NULL) return;
    // Right indent according to depth
    int i = 0;
    // for(i = 0; i < depth; ++i) printf("  ");

    // Print node data
    if(t->tag) { // non-terminal
        printf("Symbol Name: "YELLOW"%20s "RESET"| ", nonTerminalMap[t->type.nonterminal]);
        printf(YELLOW"Non Terminal "RESET"| ");
    } else { // terminal
        printf("Symbol Name: "GREEN"%20s "RESET"| ", terminalMap[t->type.terminal]);
        printf(GREEN"Terminal    "RESET" | ");
    }
    if(t->sc == NULL) { // leaf node
        // name of lexeme
        printf("Lexeme: "PURPLE"%20s "RESET"| ", t->tkn->lexeme);
        // line no.
        printf("Line no. "CYAN"%5d "RESET"| ", t->tkn->line_no);
    } else { // non-leaf node
        // typeExpression
        // printTypeExpression(t->typeExpr, t->exprType);
        // grammar rule used (TODO add actual rule as well?)
        char* s = "xxx";
        printf("Rule no. "PURPLE"%19d "RESET"| ", t->rule_no);
        printf("Line no. "CYAN"%5s "RESET"| ", s);
    }

    // node depth
    printf("Depth: "RED"%5d\n"RESET, t->depth);

    // go to children
    ParseTreeNode *temp;
    temp = t->sc;
    while(temp != NULL){
		internalPrintParseTree(temp, depth + 1);
		temp = temp->rs;
	}
}

void printParseTree(ParseTreeNode *t) {
    internalPrintParseTree(t, 0);
}
