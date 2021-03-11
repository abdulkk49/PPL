/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

#include "grammar.h"

/*This function is used to print the grammar rules to terminal
    Used for debugging purposes
    Parameters :
        G : Grammar(Array of rules)
    Returns :
        Empty return
*/
void printGrammarRules(Grammar* G){
    int i;
    printf(RED"\n\n--------------PRINTING GRAMMAR RULES--------------"RESET);


    for(i = 0; i < G->no_of_rules; i++){
        char line_out[100] = {0};
        strcpy(line_out, nonTerminalMap[G->rules[i].lhs]);
        printf(CYAN"\nRule number %2d"RESET"\n\tlhs = "YELLOW"%s"RESET"\n\trhs = ", i, line_out);
    
        SymbolNode *temp = G->rules[i].rhs.start;
        while(temp != NULL){
            if(temp->tag == 0){
                printf(GREEN"%s "RESET, terminalMap[temp->type.terminal]);
            }else{
                printf(YELLOW"%s "RESET, nonTerminalMap[temp->type.nonterminal]);
            }
            temp = temp->next;
        }
    }
    printf("\n");
}


/* This function recursively populates the RHS portion of the rule
    Parameters: 
        rl : Rule pointer
        ruleEntry : current input line stored as character array(contains everything except '\n')
        i : pointer to first T/NT
    Returns :
        Empty return
*/
void populateRHS(Rule *rl, char *ruleEntry, int i){
    if(i+1 >= strlen(ruleEntry)) //if we have reached the end of the ruleEntry
        return;

    while(ruleEntry[i] == ' ')
        i++; 

    if(ruleEntry[i] == '<'){   // if rule is non-terminal
        i++;
        char t[30] = {'\0'};
        int j=0;
        while(ruleEntry[i]!='>'){
            t[j] = ruleEntry[i];
            i++;
            j++;
        }
	
        if(rl->rhs.start == NULL){      //if the current rule has an empty rhs, 
                                        //create first node in rhs linked list
            SymbolNode *node = (SymbolNode *)malloc(sizeof(SymbolNode));
            node->next = NULL;
            node->tag = 1;
            node->type.nonterminal = findNonTerminal(t);
            rl->rhs.start = node;
            rl->rhs.len += 1;
            i++;
            populateRHS(rl, ruleEntry, i);
        }
        else{     // if current rule doesn't have an empty rhs
            SymbolNode *temp = rl->rhs.start;
            while(temp->next != NULL)
            {
                temp = temp->next;
            }
            SymbolNode *node = (SymbolNode *)malloc(sizeof(SymbolNode));
            node->next = NULL;
            node->tag = 1;
            node->type.nonterminal = findNonTerminal(t);
            temp->next = node;
            rl->rhs.len += 1;
            i++;
            populateRHS(rl, ruleEntry, i);
        }
    }
    else    // if rule is a terminal
    {
        char t[30] = {'\0'};
        int j = 0;
        while(ruleEntry[i] != ' ' && i < strlen(ruleEntry) - 1){
            t[j] = ruleEntry[i];
            i++;
            j++;
        }
        if(rl->rhs.start == NULL)           //if current rules has empty rhs
        {
            SymbolNode *node = (SymbolNode *)malloc(sizeof(SymbolNode));
            node->next = NULL;
            node->tag = 0;
            node->type.terminal = findTerminal(t);
            rl->rhs.start = node;
            rl->rhs.len += 1;
            i++;
            populateRHS(rl, ruleEntry, i);
        }
        else        //if current rule has non empty rhs, append current node to linkedlist(rhs)
        {
            SymbolNode *temp = rl->rhs.start;
            while(temp->next != NULL){
                temp = temp->next;
            }
            SymbolNode *node = (SymbolNode *)malloc(sizeof(SymbolNode));
            node->next = NULL;
            node->tag = 0;
            node->type.terminal = findTerminal(t);
            temp->next = node;
            rl->rhs.len += 1;
            i++;
            populateRHS(rl, ruleEntry, i);
            
        }
    }
}

/*This function initializes a Grammar Object
    Parameters :
        Empty parameter list
    Returns :
        G : Grammar object
*/
Grammar* initializeGrammar(){
    Grammar* G = (Grammar*)malloc(sizeof(Grammar));
    G->rules = (Rule*)malloc(sizeof(Rule)*(NO_OF_GRAMMAR_RULES+1));
    G->no_of_rules = 0;
    return G;
}

/*This function populates the Grammar G with rules written in input grammarFile
    Parameters :
        grammarFile : .txt file containing grammar rules as per defined format
        G : Array of Grammar rules
    Returns : 
        1, if successful
        0, if not successful  
*/
int populateGrammarWithRules(char *grammarFile, Grammar* G){
    FILE *inFile = fopen(grammarFile,"r");

    // Read the grammar.txt file line by line
    int gram = 0;
    char ruleEntry[150] = {'\0'};  //Contains one line of grammar.txt

    while(true){ 
        if(fgets(ruleEntry, 150, inFile) == NULL){
            printf(GREEN"\nSUCCESSFULLY READ GRAMMAR FILE : %s"RESET, grammarFile);
            return 1;
        }
        Rule rl;
        rl.rhs.len = 0;
        rl.rhs.start = NULL;
        int i = 1;    //skip i = 0; as ruleEntry[0] = '<'
        char lhs[30] = {0};
        int j = 0;
        while(ruleEntry[i] != '>'){
            lhs[j] = ruleEntry[i];
            i++;
            j++;
        }

        rl.lhs = findNonTerminal(lhs);
        i = i + 6; // to skip --> and spaces

        //reached first symbol on rhs
        populateRHS(&rl, ruleEntry, i);  // recusively populates RHS of rule 
        G->no_of_rules += 1;
        G->rules[gram++] = rl;
    }
    return 0;     
}

