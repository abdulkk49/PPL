/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

#include "lexer.h"

 /*This function reads data from file.
    Parameters :
        fp : file pointer of file from which we have to read
        s  : head pointer of linked list of tokens

    Returns :
        tkn : the token pointer if unsuccessful(error detected),
              and NULL if parsed successfully
*/

TokenDetails* tokeniseSourceCode(char* fileName, tokenStream *s){
    FILE* fp = fopen(fileName, "r");
    if(fp == NULL){
        printf(RED"\nFile not found..\n"RESET);
        return NULL;
    }
    //Caculate file size
    fseek(fp, 0, SEEK_END);
    size_t f_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    //Create an input buffer for the entire code size
    char input_buffer[f_size + 1];
    int no_of_chars_read = 0;

    //Read source file into input_buffer
    if((no_of_chars_read=(fread(input_buffer, sizeof(char), f_size, fp)))>0){
        input_buffer[no_of_chars_read]='\0';
		printf(GREEN"\nNUMBER OF BYTES READ : %d Bytes"RESET,no_of_chars_read);       
    }

    int line_no = 1; //Current line number
    char curr_lexeme[21] = {'\0'}; //Current lexeme

    for(int i = 0; strlen(input_buffer); i++){
        //Current Lexeme pointer
        int j = 0;
        //Remove all extra spaces and tabs before a lexeme
        while(input_buffer[i] != '\0' && (input_buffer[i] == ' ' 
            || input_buffer[i] == '\t'|| input_buffer[i] == '\n')){
            
            if(input_buffer[i] == '\n')
                line_no++;
            i++;
        }
        //Break if you reach the end of input_buffer(eof)
        if(input_buffer[i] == '\0') break;
        
        //Read the lexeme
        while(j <= 20 && input_buffer[i] != '\0' && input_buffer[i] != ' ' 
                && input_buffer[i] != '\n' && input_buffer[i] != '\t')
            curr_lexeme[j++] = input_buffer[i++];

        //if lexeme has length > 20, raise error
        if(j == 21){
            curr_lexeme[j-1] = '\0';
            TokenDetails* tkn = createToken(curr_lexeme, line_no);
            tkn->error = "Lexical Error : Identifier has length greater than 20";
            tkn->is_error = true;
            return tkn;
        }
    
        //if you are here, then lexeme has been read successfully
        curr_lexeme[j] = '\0';

        //Create the token for the lexeme
        TokenDetails* tkn = createToken(curr_lexeme, line_no);

        //Check if an error was detected in token creation
        //Bad token: Raise error
        if(tkn->error != NULL)
            return tkn;

        //Add good token to linkedlist
        addToken(tkn, s);
        
        //increase line_number if newline encountered
        if(input_buffer[i] == '\n') line_no++;

        //break if you reach end of input_buffer
        if(input_buffer[i] == '\0') break;
    }
    //successfully created all tokens
    return NULL;
}


 /*This function adds token to the linked list.
Parameters :- 
    node : pointer to the token
    s    : head pointer of linked list of tokens
*/
void addToken(TokenDetails *node, tokenStream *s)
{
    if(s->start == NULL)
    {
        s->start = node;
        s->end = node;
    }
    else
    {
        s->end->next = node;
        node->next = NULL;
        node->prev = s->end;
        s->end = node;
    }
}

 /*This function adds prints all tokens in the linked list.
Parameters :- 
    s    : head pointer of linked list of tokens
*/
void printTokens(tokenStream *s)
{
    printf(RED"\n--------------PRINTING TOKEN DETAILS--------------\n"RESET);
    TokenDetails *ptr = s->start;
    while(ptr != NULL)
    {
        printf("Token : "CYAN"%12s"RESET" | Lexeme : "GREEN"%12s "RESET"| Line no. : "YELLOW"%3d"RESET" | Terminal no. : "RED"%3d"RESET, ptr->token, ptr->lexeme, ptr->line_no, ptr->terminal_no);
        printf("\n");
        ptr = ptr->next;
    }

}
 /*This function checks if the lexeme is a number
Parameters :- 
    str : lexeme to be checked
*/
bool isNumeric(char* str) {
   for (int i = 0; str[i]; i++)
      if (isdigit(str[i]) == false)
      return false; //when one non numeric value is found, return false
   return true;
}

TokenDetails* createToken(char lexeme[], int line_no)
{

    //create token for the lexeme with metadata
    TokenDetails *tkn = (TokenDetails*) malloc(sizeof(TokenDetails));
    tkn->line_no = line_no;
    strcpy(tkn->lexeme, lexeme);
    tkn->error = NULL;
    tkn->is_error = false;
    tkn->is_num = false;
    tkn->next = NULL;
    tkn->prev = NULL;
    tkn->terminal_no = -1;
    
    //Check one by one for each token and map it to lexeme
    if(strcmp(lexeme, "+") == 0){
        strcpy(tkn->token, "PLUS");
        tkn->terminal_no = findTerminal("PLUS");
    }

    else if(strcmp(lexeme, "-") == 0){
        strcpy(tkn->token, "MINUS");
        tkn->terminal_no = findTerminal("MINUS");
    }

    else if(strcmp(lexeme, "*") == 0){
        strcpy(tkn->token, "MUL");
        tkn->terminal_no = findTerminal("MUL");
    }

    else if(strcmp(lexeme, "/") == 0){
        strcpy(tkn->token, "DIV");
        tkn->terminal_no = findTerminal("DIV");
    }

    else if(strcmp(lexeme, "]") == 0){
        strcpy(tkn->token, "SQBC");
        tkn->terminal_no = findTerminal("SQBC");
    }

    else if(strcmp(lexeme, "[") == 0){
        strcpy(tkn->token, "SQBO");
        tkn->terminal_no = findTerminal("SQBO");
    }

    else if(strcmp(lexeme, "}") == 0){
        strcpy(tkn->token, "CYBC");
        tkn->terminal_no = findTerminal("CYBC");
    }

    else if(strcmp(lexeme, "{") == 0){
        strcpy(tkn->token, "CYBO");
        tkn->terminal_no = findTerminal("CYBO");
    }

    else if(strcmp(lexeme, "(") == 0){
        strcpy(tkn->token, "RBO");
        tkn->terminal_no = findTerminal("RBO");
    }

    else if(strcmp(lexeme, ")") == 0){
        strcpy(tkn->token, "RBC");
        tkn->terminal_no = findTerminal("RBC");
    }

    else if(strcmp(lexeme, "program") == 0){
        strcpy(tkn->token, "PROGRAM");
        tkn->terminal_no = findTerminal("PROGRAM");
    }

    else if(strcmp(lexeme, "&&&") == 0){
        strcpy(tkn->token, "AND");
        tkn->terminal_no = findTerminal("AND");
    }

    else if(strcmp(lexeme, "|||") == 0){
        strcpy(tkn->token, "OR");
        tkn->terminal_no = findTerminal("OR");
    }

    else if(strcmp(lexeme, ":") == 0){
        strcpy(tkn->token, "COLON");
        tkn->terminal_no = findTerminal("COLON");
    }

    else if(strcmp(lexeme, ";") == 0){
        strcpy(tkn->token, "SEMICOLON");
        tkn->terminal_no = findTerminal("SEMICOLON");
    }

    else if(strcmp(lexeme, "..") == 0){
        strcpy(tkn->token, "ELLIPSIS");
        tkn->terminal_no = findTerminal("ELLIPSIS");
        
    }

    else if(strcmp(lexeme, "boolean") == 0){
        strcpy(tkn->token, "BOOLEAN");
        tkn->terminal_no = findTerminal("BOOLEAN");
    }

    else if(strcmp(lexeme, "declare") == 0){
        strcpy(tkn->token, "DECLARE");
        tkn->terminal_no = findTerminal("DECLARE");
    }

    else if(strcmp(lexeme, "integer") == 0){
        strcpy(tkn->token, "INTEGER");
        tkn->terminal_no = findTerminal("INTEGER");
    }

    else if(strcmp(lexeme, "jagged") == 0){
        strcpy(tkn->token, "JAGGED");
        tkn->terminal_no = findTerminal("JAGGED");
    }

    else if(strcmp(lexeme, "array") == 0){
        strcpy(tkn->token, "ARRAY");
        tkn->terminal_no = findTerminal("ARRAY");
    }

    else if(strcmp(lexeme, "list") == 0){
        strcpy(tkn->token, "LIST");
        tkn->terminal_no = findTerminal("LIST");
    }

    else if(strcmp(lexeme, "of") == 0){
        strcpy(tkn->token, "OF");
        tkn->terminal_no = findTerminal("OF");
    }

    else if(strcmp(lexeme, "#") == 0){
        strcpy(tkn->token, "EPSILON");
        tkn->terminal_no = findTerminal("#");
    }

    else if(strcmp(lexeme, "R1") == 0){
        strcpy(tkn->token, "R1");
        tkn->terminal_no = findTerminal("R1");
    }

    else if(strcmp(lexeme, "real") == 0){
        strcpy(tkn->token, "REAL");
        tkn->terminal_no = findTerminal("REAL");
    }

    else if(strcmp(lexeme, "size") == 0){
        strcpy(tkn->token, "SIZE");
        tkn->terminal_no = findTerminal("SIZE");
    }

    else if(strcmp(lexeme, "values") == 0){
        strcpy(tkn->token, "VALUES");
        tkn->terminal_no = findTerminal("VALUES");
    }

    else if(strcmp(lexeme, "variables") == 0){
        strcpy(tkn->token, "VARIABLES");
        tkn->terminal_no = findTerminal("VARIABLES");        
    }

    else if(strcmp(lexeme, "=") == 0){
        strcpy(tkn->token, "ASSSIGNOP");
        tkn->terminal_no = findTerminal("ASSIGNOP");        
    }

    else if(isNumeric(lexeme)){
        strcpy(tkn->token, "NUM");
        tkn->is_num = true;
        tkn->terminal_no = findTerminal("NUM");        
        tkn->value = atoi(lexeme);
    }
    
    else{ //if it is an identifier
        if(isdigit(lexeme[0])){
            tkn->is_error = true;
            tkn->error = "Lexical Error : Identifier contains first character as number";
        }
        for(int i = 0; lexeme[i]; i++){
            if(!isdigit(lexeme[i]) && !isalpha(lexeme[i]) && lexeme[i] != '_'){
                tkn->is_error = true;
                tkn->error = "Lexical Error : Identifier contains an invalid character";
                break;
            }

        }
        strcpy(tkn->token, "ID");
        tkn->terminal_no = findTerminal("ID");        

    }
    return tkn;
}
