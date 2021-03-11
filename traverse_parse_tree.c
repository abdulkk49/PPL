#include "traverse_parse_tree.h"
#include "hashmap.h"

char* getDataType(ParseTreeNode* nodeCheck){
    if(nodeCheck->type.nonterminal == primDatatype){
        if(nodeCheck->sc->type.terminal == INTEGER) 
            return "integer";
        else if(nodeCheck->sc->type.terminal == REAL) 
            return "real";
        else
            return "boolean";
    }
    else if(nodeCheck->type.nonterminal == arrDatatype){
        return "array";
    }
    //else return jagged
    return "jagged";
}

ParseTreeNode* shiftRight(ParseTreeNode* node, int shift){
    while(shift>0){
        if(node==NULL)
            return NULL;
        node = node->rs;
        shift--;
    }
    return node;
}

void findArrayDims(ParseTreeNode* node, bool* hasDynamic, int* numberOfDimensions, int i, char r1[21][21], char r2[21][21]){
    *numberOfDimensions += 1;
    node = node->sc->rs->sc; //ID / NUM
    if(node->type.terminal == ID){
        *hasDynamic = true;
        strcpy(r1[i], node->tkn->lexeme);
    }
    else{
        int x = node->tkn->value;
        int length = snprintf( NULL, 0, "%d", x);
        char* str = (char*)malloc(length + 1 );
        snprintf( str, length + 1, "%d", x);
        strcpy(r1[i], str);
        free(str);
    }
    node = node->parent->rs->rs->sc; //ID / NUM
    if(node->type.terminal == ID){
        *hasDynamic = true;
        strcpy(r2[i], node->tkn->lexeme);
    }
    else{
        int x = node->tkn->value;
        int length = snprintf( NULL, 0, "%d", x);
        char* str = (char*)malloc(length + 1);
        snprintf(str, length + 1, "%d", x);
        strcpy(r2[i], str);
        free(str);
    }
    node = node->parent->rs->rs;
    
    if(node != NULL)
        findArrayDims(node, hasDynamic, numberOfDimensions, i+1, r1, r2);
    return;
}

void shallowCopy(tableEntry* t, tableEntry* similarEntry){
    similarEntry->type = t->type;
    strcpy(similarEntry->arrayType, t->arrayType);
    // printf("\n'''''\n%s \n", similarEntry->arrayType);
    if(t->type == 0){
        similarEntry->typeExpr.primitive = t->typeExpr.primitive;
    }
    else if(t->type == 1){
        similarEntry->typeExpr.rectArray.dimensions = t->typeExpr.rectArray.dimensions;
        similarEntry->typeExpr.rectArray.basicElementType = t->typeExpr.rectArray.basicElementType;
        similarEntry->typeExpr.rectArray.range1 = t->typeExpr.rectArray.range1;
        similarEntry->typeExpr.rectArray.range2 = t->typeExpr.rectArray.range2;
    }
    else{
        similarEntry->typeExpr.jaggedArray.dimensions = t->typeExpr.jaggedArray.dimensions;
        similarEntry->typeExpr.jaggedArray.basicElementType = t->typeExpr.jaggedArray.basicElementType;
        similarEntry->typeExpr.jaggedArray.range2_2D = t->typeExpr.jaggedArray.range2_2D;
        similarEntry->typeExpr.jaggedArray.range2_3D = t->typeExpr.jaggedArray.range2_3D;
        // printf("2D\n--%d %d\n", similarEntry->typeExpr.jaggedArray.range2_2D[0], similarEntry->typeExpr.jaggedArray.range2_2D[1]);
        similarEntry->typeExpr.jaggedArray.range1[0] = t->typeExpr.jaggedArray.range1[0];
        similarEntry->typeExpr.jaggedArray.range1[1] = t->typeExpr.jaggedArray.range1[1];
        // getchar();
        // printf("3D%d %d\n", similarEntry->typeExpr.jaggedArray.range2_3D[0][0], similarEntry->typeExpr.jaggedArray.range2_3D[1][1]);
    }

}

bool check2DRecursive(ParseTreeNode* node, int *j){
    node = node->sc; //<numList> / SEMICOLON / #
    // printDebug(node);
    if(node->tag == 0 && node->type.terminal == EPSILON){
        printf(RED"Error : 2D Jagged array declaration error at line %d\n"RESET, node->tkn->line_no);
        return false;
    }
    else if(node->tag == 0 && node->type.terminal == SEMICOLON){
        printf(RED"Error : 2D Jagged array declaration error at line %d\n"RESET, node->tkn->line_no);
        return false;
    }
    //<numList>
    if(node->rs == NULL){
        node = node->sc;
        *j += 1;
        if(node->rs != NULL){
            printf(RED"Error : 3D declaration List of numbers for 2D array at line %d\n"RESET, node->tkn->line_no);
            return false;
        }
        return true;
    }
    
    else{
        node = node->sc;
        *j += 1;
        if(node->rs != NULL){
            printf(RED"Error : 3D declaration List of numbers for 2D array at line %d\n"RESET, node->tkn->line_no);
            return false;
        }
        node = node->parent->rs->rs; //<jaggedValues>
        // printDebug(node);
    }
    return check2DRecursive(node, j);
}

bool check3DRecursive(ParseTreeNode* node, int *j, int* R3){
    node = node->sc; //<numList> / SEMICOLON / #
    // printDebug(node);
    if(node->tag == 0 && node->type.terminal == EPSILON){           //#
        printf(RED"Error : 3D Jagged array declaration error at line %d\n"RESET, node->tkn->line_no);
        return false;
    }
    else if(node->tag == 0 && node->type.terminal == SEMICOLON){    //Semicolon
        printf(RED"Error : 3D Jagged array declaration error at line %d\n"RESET, node->tkn->line_no);
        return false;
    }
    else if(node->rs == NULL){              //ending <numList>
        int counter = 1;
        node = node->sc;
        while(node->rs!=NULL){
            counter++;
            node = node->rs->sc;
        }
        // printf("End list\n");
        R3[*j] = counter;
        // printf("Size %d -- %d\n", *j, counter);
        *j += 1;
        return true;
    }
    else{
        int counter = 1;
        ParseTreeNode* temp = node;
        node = node->sc;
        while(node->rs!=NULL){
            counter++;
            node = node->rs->sc;
        }
        // printf("Size %d -- %d\n", *j, counter);
        R3[*j] = counter;
        *j += 1;
        node = temp->rs->rs; //<jaggedValues>
        // printDebug(node);
    }
    return check3DRecursive(node, j, R3);
}

bool assignTypeExpr(tableEntry* t, ParseTreeNode* node, char* dt, tableEntry** T){
    //dt = "real"
    if(strcmp(dt, "real") == 0){
        t->type = 0;
        t->typeExpr.primitive = real;
        //Add t to LinkedList of tableEntries
        addtoTable(t, T);
        return true;
    }
    else if(strcmp(dt, "integer") == 0){
        t->type = 0;
        t->typeExpr.primitive = integer;
        //Add t to LinkedList of tableEntries
        addtoTable(t, T);
        return true;
    }
    else if(strcmp(dt, "boolean") == 0){
        t->type = 0;
        t->typeExpr.primitive = boolean;
        //Add t to LinkedList of tableEntries
        addtoTable(t, T);
        return true;
    }
    else if(strcmp(dt, "array") == 0){
        t->type = 1;

        bool hasDynamic = false;
        int numberOfDimensions = 0;
        char r1[MAX_ALLOWED_DIMENSION][21] = {0};
        char r2[MAX_ALLOWED_DIMENSION][21] = {0};
        //Recursively populate low and high values
        //Also calculate the number of dimensions
        node = node->sc->rs;
        findArrayDims(node, &hasDynamic, &numberOfDimensions, 0, r1, r2);
        
        // printf("Number of Dimensions is %d\n", numberOfDimensions);
        t->typeExpr.rectArray.dimensions = numberOfDimensions;
        t->typeExpr.rectArray.basicElementType = integer;
        t->typeExpr.rectArray.range1 = (char**)malloc(sizeof(char*)*numberOfDimensions);
        t->typeExpr.rectArray.range2 = (char**)malloc(sizeof(char*)*numberOfDimensions);
    
        for(int i = 0; i < numberOfDimensions; i++){
            t->typeExpr.rectArray.range1[i] = (char*)malloc(sizeof(char)*(strlen(r1[i]) + 2));
            strcpy(t->typeExpr.rectArray.range1[i], r1[i]);
            // printf("%s\n", t->typeExpr.rectArray.range1[i]);
        }
        for(int i = 0; i < numberOfDimensions; i++){
            t->typeExpr.rectArray.range2[i] = (char*)malloc(sizeof(char)*(strlen(r2[i]) + 1));
            strcpy(t->typeExpr.rectArray.range2[i], r2[i]);
            // printf("%s\n", t->typeExpr.rectArray.range2[i]);
        }

        if(hasDynamic){
            strcpy(t->arrayType, "Dynamic");
        }
        else{
            strcpy(t->arrayType, "Static");
        }
        //Add t to LinkedList of tableEntries
        
        addtoTable(t, T);
        return true;
    }
    if(strcmp(dt, "jagged") == 0){
        t->type = 2;
        node = node->sc; 
        // printf("Inside AssignType Expression\n");
        // printDebug(node);            //pointing at terminal JAGGED
        
        int R1_temp[2];
        while(node->type.terminal != NUM)                   //pointing at first num value
            node = node->rs;
        // printDebug(node);  

        R1_temp[0] = node->tkn->value;               //pointing at first num value
        if(R1_temp[0] < 0){
            printf(RED"Error : Jagged array declaration error at line %d : Negative Index\n"RESET, node->tkn->line_no);
            return false;
        }
        node = node->rs->rs;                      //pointing at second num value
        
        // printDebug(node);  
        R1_temp[1] = node->tkn->value;
        if(R1_temp[1] < 0){
            printf(RED"Error : Jagged array declaration error at line %d : Negative Index\n"RESET, node->tkn->line_no);
            return false;
        }
        t->typeExpr.jaggedArray.range1[0] = R1_temp[0];           //assigning R1
        t->typeExpr.jaggedArray.range1[1] = R1_temp[1];           //assigning R1
        t->typeExpr.jaggedArray.basicElementType = integer;
        strcpy(t->arrayType, "Static");
        while(node->tag != 1)   //move to <option3D>
            node = node->rs;

        // printDebug(node);  

        if(node->sc->type.terminal == EPSILON){     //2D jagged
            t->typeExpr.jaggedArray.dimensions = 2;
            // printDebug(node->sc);
            // printDebug(node->parent);
            node = node->parent->rs->rs; //<jaggedinit>
            // printDebug(node);  
            node = node->sc; //R1
            // printDebug(node);  
            int rows = R1_temp[1] - R1_temp[0] + 1;
            // printf("rows = %d ", rows);
            int i = 0;
            int *R2_temp = (int*)malloc(rows*sizeof(int));     //allocating memory for R2
            int prev = -1;
            int line_no;
            while(i < rows){
                node = node->rs->rs;
                // printDebug(node);  
                int index = node->tkn->value;
                line_no = node->tkn->line_no;
                // printf("index = %d ", index);
                if(prev != -1 && index != prev + 1){
                    printf(RED"Error : Non contnuous index assignment in 2D jagged array at line %d\n"RESET, line_no);
                    return false;
                }
                if(index <= R1_temp[1] && index >= R1_temp[0]){
                    node = node->rs->rs->rs->rs;
                    // printf("cols = %d\n", cols);
                    int cols = node->tkn->value;
                    R2_temp[i] = cols;
                    if(cols < 0){
                        printf(RED"Error : 2D jagged array declaration error at line %d : Negative Column index\n"RESET, line_no);
                        return false;
                    }
                    int j = 0; 
                    while(node->tag != 1) //<jaggedValues>
                        node = node->rs;
                    // printDebug(node);  
                    bool success = check2DRecursive(node, &j);
                    if(!success) return false;
                    if(j != cols){
                        printf(RED"Error : 2D Jagged array declaration error at line %d : \n"RESET, line_no);
                        return false;
                    }  
                }
                else{
                    printf(RED"Error : 2D Jagged array declaration error at line %d : Index out of bounds\n"RESET, line_no);
                    return false;
                }
                i++;
                if(node->rs->rs == NULL)
                    break;
                node = node->rs->rs->sc;
                prev = index;
            }
            if(i != rows && node->rs->rs == NULL){
                printf("Error : 2D Jagged array declaration error at line %d\n", line_no);
                return false;

            }
            t->typeExpr.jaggedArray.range2_2D = R2_temp;
            t->typeExpr.jaggedArray.range2_3D = NULL;
            addtoTable(t, T);
            return true;
        }
        else{      
            // printf("Inside 3D..\n");                                         //3D jagged
            t->typeExpr.jaggedArray.dimensions = 3;
            int rows = R1_temp[1]- R1_temp[0] + 1;
            int i = 0;
            int *R2_temp = (int*)malloc(rows*sizeof(int));    //allocating memory for R2
            int **R3_temp = (int**)malloc(rows*sizeof(int*));//allocating memory for R3
            node = node->parent->rs->rs; //<jaggedinit>
            int prev = -1;
            // printDebug(node);
            node = node->sc; //R1
            // printDebug(node);
            int line_no;
            while(i < rows){
                node = node->rs->rs; //NUM(Index)
                // printDebug(node);
                int index = node->tkn->value;
                line_no = node->tkn->line_no;
                // printf("index = %d \n", index);
                if(prev != -1 && index != prev + 1){
                    printf("Error : 3D Jagged array declaration error at %d\n", line_no);
                    return false;
                }
                if(index <= R1_temp[1] && index >= R1_temp[0]){
                    int cols = node->rs->rs->rs->rs->tkn->value;//NUM(size)
                    // printDebug(node);
                    // printf("cols = %d \n", cols);
                    R2_temp[i] = cols;
                    R3_temp[i] = (int*)malloc(sizeof(int)*cols);
                    if(cols < 0){
                        printf("Error : 3D Jagged array declaration error at %d\n", line_no);
                        return false;
                    }
                    int j = 0; 
                    while(node->tag != 1) //<jaggedValues>
                        node = node->rs;
                    // printDebug(node);
                    bool success = check3DRecursive(node, &j, R3_temp[i]);
                    if(j != cols || !success){
                        printf("Error : 3D Jagged array declaration error at %d\n", line_no);
                        return false;
                    }  
                }
                else{
                    printf("Error : 3D Jagged array declaration error at %d\n", line_no);
                    return false;
                }
                i++;
                if(node->rs->rs == NULL)
                    break;
                node = node->rs->rs->sc;
                prev = index;
            }
            if(i != rows && node->rs->rs == NULL){
                printf("Error : 3D Jagged array declaration error at %d\n", line_no);
                return false;
            }
            t->typeExpr.jaggedArray.range2_2D = R2_temp;
            t->typeExpr.jaggedArray.range2_3D = R3_temp;
            addtoTable(t, T);
            return true;
        }
    }
}


bool addDeclaration(ParseTreeNode* declPointer, tableEntry** T){
    //declPointer points to <declaration>
    //Case1 : DECLARE ID : [etc]
    //Case2 : <commonList> [etc]
    ParseTreeNode* nodeCheck = declPointer->sc;
    // printDebug(nodeCheck);
    if(nodeCheck->tag == 0){ //DECLARE (Rules 4, 5, 6)
        
        //Save the name of identifier for tableEntry
        char name[21];
        strcpy(name, nodeCheck->rs->tkn->lexeme);
        // printf("%s\n", name);
        //move to <primDatatype> or <arrDatatype> or <jarrDatatype>
        //any of these 3 is possible
        while(nodeCheck->tag != 1){
            nodeCheck = nodeCheck->rs;
        }
        // printDebug(nodeCheck);
        tableEntry* t = createTableEntry(-1, name, "not_applicable");
        //get appropriate datatype using above non teminal
        //for one of 3 cases
        //nodeCheck points to <primDatatype>/<arrDatatype>/<jarrDatatype>
        char datatype[10] = {0};
        strcpy(datatype, getDataType(nodeCheck));
        // printf("dt = %s", datatype);
        // getchar();
        bool isAdded = assignTypeExpr(t, nodeCheck, datatype, T);
        if(!isAdded) return false;
        return true;
    }
    //Case2 : <commonList> [etc]
    else{ // if it is LIST
        ParseTreeNode* temp = nodeCheck;
        nodeCheck = nodeCheck->sc;
        while(nodeCheck->tag != 1) //move to <IDList>
            nodeCheck = nodeCheck->rs;
        
        char varnames[MAX_ALLOWED_DECLARATIONS][21];
        //move to ID
        nodeCheck = nodeCheck->sc;
        strcpy(varnames[0], nodeCheck->tkn->lexeme);
        //move to next ID
        nodeCheck = nodeCheck->rs;
        int i = 1;
        while(i < MAX_ALLOWED_DECLARATIONS - 1 && nodeCheck->type.terminal != EPSILON){
            strcpy(varnames[i], nodeCheck->tkn->lexeme);
            i++;
            nodeCheck = nodeCheck->rs->sc;
        }

        nodeCheck = temp;
        nodeCheck = nodeCheck->rs;

        char* datatype = getDataType(nodeCheck);
        tableEntry* t = createTableEntry(-1, varnames[0], "not_applicable"); 
        bool isAdded = assignTypeExpr(t, nodeCheck, datatype, T);
        if(!isAdded) return false;
        // printf("%d variables in the list\n", i);
        int count = 1;
        while(count < i){
            tableEntry* similarEntry = createTableEntry(-1, varnames[count], "not_applicable"); 
            shallowCopy(t, similarEntry);
            addtoTable(similarEntry, T);
            count++;
        }
        return true;
    }
}

void addDeclarationsToTable(ParseTreeNode* declPointer, tableEntry** T){
    //add all <declarations> to tableEntry** T
    while(declPointer){
        if(declPointer->sc){
            //move to <declaration>
            declPointer = declPointer->sc;
            // printDebug(declPointer);
            addDeclaration(declPointer, T);
        }
        declPointer = declPointer->rs;
    }
}


/*----------------------------------------------------
-------------------------------------------------------*/

char *typeMap [] = {
    "PRIMITIVE",
    "RECTANGULAR ARRAY",
    "JAGGED ARRAY"
};

// always for assignment
// void printError(ParseTreeNode* node, Terminal op, tableEntry* l, tableEntry* r, char* msg) {
//     printf("Line no. %d -- ASSIGNMENT -- ", node->tkn->line_no);
//     printf("Operator: %s -- ", terminalMap[op]);
//     printf("(%s, %s) -- (%s, %s) -- ", l->name, typeMap[l->type], r->name, typeMap[r->type]);
//     printf("Depth: %d -- ", node->depth);
//     printf("%s\n", msg);
// }

// Terminal getOp(ParseTreeNode* node){
//     if(node->type.terminal == PLUS)
//         return PLUS;
//     if(node->type.terminal == MINUS)
//         return MINUS;
//     if(node->type.terminal == DIV)
//         return DIV;
//     if(node->type.terminal == MUL)
//         return MUL;
//     if(node->type.terminal == AND)
//         return AND;
//     if(node->type.terminal == OR)
//         return OR;
// }
// tableEntry* checkArrayCompatibility(tableEntry *l, tableEntry *r, ParseTreeNode* node, Terminal term, tableEntry **T){
//     if(l->type == 1){ //if rectangular
//         if(strcmp(l->arrayType, "Dynamic") == 0 || strcmp(r->arrayType, "Dynamic") == 0){
//             for(int i = 0; i < l->typeExpr.rectArray.dimensions; i++){
//                 tableEntry *t = lookup(l->typeExpr.rectArray.range1[i], T);
//                 if(t == NULL) continue;
//                 if(t->type != 0){
//                     printError(node, term, l, r, "Error : Rect array range has a non primitive low value");
//                     return NULL;
//                 }
//                 if(t->type == 0 && t->typeExpr.primitive != integer){
//                     printError(node, term, l, r, "Error : Rect array range has a non integer low value");
//                     return NULL;
//                 }
//             }
//             for(int i = 0; i < l->typeExpr.rectArray.dimensions; i++){
//                 tableEntry *t = lookup(l->typeExpr.rectArray.range2[i], T);
//                 if(t == NULL) continue;
//                 if(t->type != 0){
//                     printError(node, term, l, r, "Error : Rect array range has a non primitive high value");
//                     return NULL;
//                 }
//                 if(t->type == 0 && t->typeExpr.primitive != integer){
//                     printError(node, term, l, r, "Error : Rect array range has a non integer high value");
//                     return NULL;
//                 }
//             }
//             for(int i = 0; i < r->typeExpr.rectArray.dimensions; i++){
//                 tableEntry *t = lookup(r->typeExpr.rectArray.range1[i], T);
//                 if(t == NULL) continue;
//                 if(t->type != 0){
//                     printError(node, term, l, r, "Error : Rect array range has a non primitive low value");
//                     return NULL;
//                 }
//                 if(t->type == 0 && t->typeExpr.primitive != integer){
//                     printError(node, term, l, r, "Error : Rect array range has a non integer low value");
//                     return NULL;
//                 }
//             }
//             for(int i = 0; i < r->typeExpr.rectArray.dimensions; i++){
//                 tableEntry *t = lookup(r->typeExpr.rectArray.range2[i], T);
//                 if(t == NULL) continue; //if it is a num
//                 if(t->type != 0){
//                     printError(node, term, l, r, "Error : Rect array range has a non primitive low value");
//                     return NULL;
//                 }
//                 if(t->type == 0 && t->typeExpr.primitive != integer){
//                     printError(node, term, l, r, "Error : Rect array range has a non integer low value");
//                     return NULL;
//                 }
//             }
//             return l;
//         } 
//         if(l->typeExpr.rectArray.dimensions == r->typeExpr.rectArray.dimensions){
//             for(int i = 0; i < l->typeExpr.rectArray.dimensions; i++){
//                 if(strcmp(l->typeExpr.rectArray.range1[i], r->typeExpr.rectArray.range1[i]) != 0){
//                     printError(node, term, l, r, "Rect array ranges are different");
//                     return NULL;
//                 }
//                 if(strcmp(l->typeExpr.rectArray.range2[i], r->typeExpr.rectArray.range2[i]) != 0){
//                     printError(node, term, l, r, "Rect array ranges are different");
//                     return NULL;
//                 }
//             }
//         }
//         else{
//             printError(node, term, l, r, "Rect array dimensions are different");
//                     return NULL;
//         }
//     }
//     else{//if jagged
//         if(l->typeExpr.jaggedArray.dimensions == r->typeExpr.jaggedArray.dimensions){
//             for(int i = 0; i < 2; i++){
//                 if(l->typeExpr.jaggedArray.range1[i] != r->typeExpr.jaggedArray.range1[i]){
//                     printError(node, term, l, r, "Jagged array ranges are different");
//                     return NULL;
//                 }   
//             }
//             int rows = l->typeExpr.jaggedArray.range1[1] - l->typeExpr.jaggedArray.range1[0] + 1;
//             for(int i = 0; i < rows; i++){
//                 if(l->typeExpr.jaggedArray.range2_2D[i] != r->typeExpr.jaggedArray.range2_2D[i]){
//                     printError(node, term, l, r, "Jagged array row sizes are different");
//                     return NULL;
//                 }
//             }
//             if(l->typeExpr.jaggedArray.range2_3D == NULL && r->typeExpr.jaggedArray.range2_3D == NULL){
//                 return l;
//             }
//             //both 3D
//             for(int i = 0; i < rows; i++){
//                 for(int j = 0; j < l->typeExpr.jaggedArray.range2_2D[i]; j++){
//                     if(l->typeExpr.jaggedArray.range2_3D[i][j] != r->typeExpr.jaggedArray.range2_3D[i][j]){
//                         printError(node, term, l, r, "3D Jagged arrays have different depth sizes");
//                         return NULL;
//                     }
//                 }
//             }
//         }
//         else{
//             printError(node, term, l, r, "2D and 3D Jagged array cannot be operands");
//             return NULL;
//         }       
//     }
//     return l;
// }
//takes <indices> NT as input along with type of the ID
void checkArrayBounds(ParseTreeNode* node, tableEntry *t){
    
    // if(t->type == 1){
    //     int index;
    //     int dim = t->typeExpr.rectArray.dimensions;
    //     //getting all values of indices dynamic/static
    //     for(int i = 0; i<dim; i++){
    //         if(node==NULL)       //less number of indices provided than the actual dimension
    //             // printError();
    //         node = node->sc;   //<IDNum>
    //         if(node->sc->type.nonterminal == ID){
    //             // printError();   //dynamic
    //         }
    //         else
    //         //What is indices????
    //             index = node->sc->tkn->value;
    //         if(indices[0] < atoi(t->typeExpr.rectArray.range1[i]) || indices[0] > atoi(t->typeExpr.rectArray.range2[i])) //out of bounds
    //             // printError();
    //         node = node->rs;     //<indices>
    //     }
    //     if(node!=NULL){
    //         // printError();       //more number of indices provided than the actual dimension
    //     }
    // }
    
    // else if(t->type == 2){
    //     int indices[3];
    //     int dim = t->typeExpr.jaggedArray.dimensions;
    //     //getting all values of indices dynamic/static
    //     for(int i = 0; i<dim; i++){
    //         node = node->sc;   //<IDNum>
    //         if(node->sc.type.nonterminal == ID){
    //             printError();   //dynamic
    //         }
    //         else
    //             indices[i] = node->sc->tkn->value;
    //         node = node->rs;     //<indices>
    //         if(node==NULL)       //less number of indices provided than the actual dimension
    //             printError();
    //     }
    //     if(node!=NULL)
    //         printError();       //more number of indices provided than the actual dimension
    //     if(indices[0] < t->typeExpr.jaggedArray.range1[0] || indices[0] > typeExpr.jaggedArray.range1[1])
    //         printError();       //out of bounds of R1
    //     if(dim == 2){
    //         if(indices[1] < 1 || indices[1] > typeExpr.jaggedArray.range2_2D[indices[0]-t->typeExpr.jaggedArray.range1[0]])
    //             printError();   //out of bounds of R2
    //     }
    //     else if(dim == 3){
    //         if(indices[1] < 1 || indices[1] > typeExpr.jaggedArray.range2_3D[indices[1]-t->typeExpr.jaggedArray.range1[0]][0])
    //             printError();   //out of bounds of R2
    //         if(indices[2] < 1 || indices[2] > typeExpr.jaggedArray.range2_3D[indices[1]-t->typeExpr.jaggedArray.range1[0]][t->typeExpr.jaggedArray.range2_3D[indices[1]-t->typeExpr.jaggedArray.range1[0]-1][0]])\
    //             printError();   //out of bounds of R3
    //     }
    // }
}


// tableEntry* commonCheck(tableEntry *l, tableEntry *r, ParseTreeNode* nodeCheck, Terminal t, tableEntry **T){
//     //if both of them are rect arrays
//     if(l->type == 1 && r->type == 1){
//         return checkArrayCompatibility(l,r,nodeCheck, t, T);
//     }
//     //if both are jagged arrays
//     else if(l->type == 2 && r->type == 2){
//         return checkArrayCompatibility(l,r,nodeCheck,t, T);
//     }
//     //if one is jagged and other is rectangular
//     else if((l->type == 1 && r->type == 2)||(l->type == 2 && r->type == 1)){
//         printError(nodeCheck, t, l, r, "Incompatible Array types(jagged and rectangular");
//         return NULL; //any
//     }
// }
// tableEntry* getTypeOfFactor(ParseTreeNode* node, tableEntry **T){
//     node = node->sc;
//     if(node->type.terminal == ID){
//         tableEntry *t = lookup(node->tkn->lexeme, T);
//         if(node->rs == NULL){ //If it is some primitive type or array type
//             return t;
//         }
//         else{ // if it is an array element, it can only be an integer
//             checkArrayBounds(node->rs->rs, t);
//             tableEntry *tq = createTableEntry(0, node->tkn->lexeme, "not_applicable");
//             tq->typeExpr.primitive = integer;
//             return tq;
//         }

//     }
//     else if(node->type.terminal == NUM){ //if it is a number, can only be an integer
//         tableEntry *t = createTableEntry(0, "number", "not_applicable");
//         t->typeExpr.primitive = integer;
//         return t;
//     }
// }
// tableEntry* getTypeOfExtendTerm(ParseTreeNode* node, tableEntry **T){
//     if(node->sc->tag == 0 && node->sc->type.terminal == EPSILON)
//         return NULL;
//     //node = node->sc->rs; //<factor>
//     tableEntry *l = getTypeOfFactor(node->sc->rs, T);
//     tableEntry* r = getTypeOfExtendTerm(node->sc->rs->rs, T);
//     if(r == NULL)
//         return l;
//     //if array element/ ID / NUM
//     node = node->sc->sc; //<opMulDiv>
//     Terminal operator = getOp(node);
//     tableEntry* check = commonCheck(l, r, node, operator, T);
//     if(check == NULL) return NULL;

//     if(operator == MUL){
//         if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//             printError(node, operator, l, r, "Error: Boolean operands for MUL operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(node, operator, l, r, "Error: Incompatible operand types for MUL operator");
//             return NULL;
//         }
//         return l;
//     }
//     else{
//         if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//             printError(node, operator, l, r, "Error: Boolean operands for DIV operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(node, operator, l, r, "Error: Incompatible operand types for DIV operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive == integer){
//             tableEntry *t;
//             shallowCopy(l, t);
//             t->typeExpr.primitive = real;
//             return t;
//         }
//         return l;
//     }
//     return l;
// }


// tableEntry* getTypeOfTerm(ParseTreeNode* node, tableEntry** T){
//     node = node->sc;
//     tableEntry *l = getTypeOfFactor(node, T);
//     node = node->rs;
//     tableEntry *r = getTypeOfExtendTerm(node, T);
//     if(r == NULL)
//         return l;
//     node = node->sc->sc;
//     if(r == NULL)
//         return l;
//     //if array element/ ID / NUM
//     node = node->sc->sc; //<opMulDiv>
//     Terminal operator = getOp(node);
//     tableEntry* check = commonCheck(l, r, node, operator, T);
//     if(check == NULL) return NULL;

//     if(operator == MUL){
//         if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//             printError(node, operator, l, r, "Error: Boolean operands for MUL operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(node, operator, l, r, "Error: Incompatible operand types for MUL operator");
//             return NULL;
//         }
//         return l;
//     }
//     else{
//         if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//             printError(node, operator, l, r, "Error: Boolean operands for DIV operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(node, operator, l, r, "Error: Incompatible operand types for DIV operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive == integer){
//             tableEntry *t;
//             shallowCopy(l, t);
//             t->typeExpr.primitive = real;
//             return t;
//         }
//         return l;
//     }
//     return l;

// }

// tableEntry* getTypeOfExtendArithExpr(ParseTreeNode* node, tableEntry **T){
//     if(node->sc->tag == 0 && node->sc->type.terminal == EPSILON)
//         return NULL;
//     //node = node->sc->rs; //<factor>
//     tableEntry *l = getTypeOfTerm(node->sc->rs, T);
//     tableEntry* r = getTypeOfExtendArithExpr(node->sc->rs->rs, T);
//     if(r == NULL)
//         return l; 
//     node = node->sc->sc; //<opMulDiv>
//     Terminal operator = getOp(node);
//     tableEntry* check = commonCheck(l, r, node, operator, T);
//     if(check == NULL) return NULL;
    
//     if(operator == PLUS){
//         if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//             printError(node, operator, l, r, "Error : Boolean operands for PLUS operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(node, operator, l, r, "Error : Incompatible operand types for PLUS operator");
//             return NULL;
//         }
//         return l;
//     }
//     else{
//         if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//             printError(node, operator, l, r, "Error : Boolean operands for MINUS operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(node, operator, l, r, "Error : Incompatible operand types for MINUS operator");
//             return NULL;
//         }
//         return l;
//     }
//     return l;
// }



// tableEntry* getTypeOfBoolean(ParseTreeNode* node, tableEntry **T){
//     if(node->sc->tag == 0)
//         return NULL;
//     tableEntry* l = getTypeOfFactor(node->sc->rs, T);
//     tableEntry* r = getTypeOfBoolean(node->sc->rs->rs, T);
//     if(r == NULL) return l;
//     node = node->sc->sc;
//     Terminal operator = getOp(node);
//     if(l->type != 0 || r->type != 0){
//         printError(node, operator,l,r,"Operands are not of type boolean for logical operator");
//         return NULL;
//     }
//      if(l->typeExpr.primitive != boolean  || r->typeExpr.primitive != boolean){
//         printError(node, operator, l,r,"Operands are not of type boolean for logical operator");
//         return NULL;
//     }
//     return l;
    
// }

// tableEntry* getTypeOfExpression(ParseTreeNode* node, tableEntry **T){
//     //if arithmetic
//     tableEntry* temp;
//     if(node->sc->rs->sc->tag == 0){ //if <<booleanExpr> = EPSILON
//         node = node->sc; //<arithmeticExpr>
//         node = node->sc; //<term>
//         //node = node->sc->rs; //<factor>
//         tableEntry *l = getTypeOfTerm(node, T);
//         tableEntry* r = getTypeOfExtendArithExpr(node->rs, T);
//         temp = l;
//         if(r == NULL)
//             return l; 
//         node = node->sc->sc; //<opMulDiv>
//         Terminal operator = getOp(node);
//         tableEntry* check = commonCheck(l, r, node, operator, T);
//         if(check == NULL) return NULL;
        
//         if(operator == PLUS){
//             if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//                 printError(node, operator, l, r, "Error : Boolean operands for PLUS operator");
//                 return NULL;
//             }
//             if(l->typeExpr.primitive != r->typeExpr.primitive){
//                 printError(node, operator, l, r, "Error : Incompatible operand types for PLUS operator");
//                 return NULL;
//             }
//             return l;
//         }
//         else{
//             if(l->typeExpr.primitive == boolean || r->typeExpr.primitive == boolean){
//                 printError(node, operator, l, r, "Error : Boolean operands for MINUS operator");
//                 return NULL;
//             }
//             if(l->typeExpr.primitive != r->typeExpr.primitive){
//                 printError(node, operator, l, r, "Error : Incompatible operand types for MINUS operator");
//                 return NULL;
//             }
//             return l;
//         }
                
//     }
//     else{ //if boolean
        
//         tableEntry* l = getTypeOfFactor(node->sc->sc->sc, T);

//         node = node->sc->rs;
//         tableEntry* r = getTypeOfBoolean(node, T);
//         if(r == NULL) return l;
//         node = node->sc->sc;
//         Terminal operator = getOp(node);
//         if(l->type != 0 || r->type != 0){
//             printError(node, operator,l,r,"Operands are not of type boolean for logical operator");
//             return NULL;
//         }
//         if(l->typeExpr.primitive != boolean  || r->typeExpr.primitive != boolean){
//             printError(node, operator, l,r,"Operands are not of type boolean for logical operator");
//             return NULL;
//         }
//     }
    
//     return temp;
// }

// void checkAssignment(ParseTreeNode* assignPointer, tableEntry **T){
//     //assign pointer points to <assignment>
//     ParseTreeNode* nodeCheck = assignPointer->sc; //ID
    
//     // printDebug(nodeCheck);
    
//     tableEntry* l = lookup(nodeCheck->tkn->lexeme, T);
//     //move to <LvalueID>/<lvalueArr>
//     nodeCheck = nodeCheck->rs->sc;
//     tableEntry *r = NULL;
//     // printDebug(nodeCheck);
//     //if array or primitive
//     if(nodeCheck->type.nonterminal == lvalueID){        
//         nodeCheck = nodeCheck->sc->rs;
//         // printDebug(nodeCheck);
//         r = getTypeOfExpression(nodeCheck, T);
//         //if not array
//         tableEntry * check = commonCheck(l, r, nodeCheck, ASSIGNOP, T);
//         if(check == NULL) return;
//     }
//     else{
//         //move to <indices>
        
//         nodeCheck = nodeCheck->sc->rs;

//         // printDebug(nodeCheck);

//         checkArrayBounds(nodeCheck, l);
//         //move to <expression>
//         while(nodeCheck->tag != 1)
//             nodeCheck = nodeCheck->rs;
        
//         // printDebug(nodeCheck);


//         l = createTableEntry(0, l->name, "not_applicable");
//         l->typeExpr.primitive = integer;
        
//         r = getTypeOfExpression(nodeCheck, T);
//         commonCheck(l, r, nodeCheck, ASSIGNOP, T);
//         //TODO Check
//         if(l->typeExpr.primitive != r->typeExpr.primitive){
//             printError(nodeCheck, ASSIGNOP, l, r, "Incompatible Primitive Data type assigned to array element");
//             return;
//         }
//     }
    
//     return;
 
// }

// void checkAssignmentsFromTable(ParseTreeNode* assignPointer, tableEntry** T){
//     while(assignPointer){
//         if(assignPointer->sc){
//             //move to <assignment>
//             assignPointer = assignPointer->sc;
//             // printDebug(assignPointer);
//             checkAssignment(assignPointer, T);
//         }
//         assignPointer = assignPointer->rs;
//     }
// }
















void traverseParseTree(ParseTreeNode* root, tableEntry** T){
    ParseTreeNode* temp = root->sc;
    while(temp->tag != 1){ //move till <declarations> in parse tree
        temp = temp->rs;
    }
    // printDebug(temp);
    //points to <declarations> in parse tree
    ParseTreeNode* declPointer = temp;
    addDeclarationsToTable(declPointer, T);

    //points to <assignments> in parse tree
    // ParseTreeNode* assgnPointer = temp->rs;
    // checkAssignmentsFromTable(assgnPointer, T);
}
void printDebug(ParseTreeNode* t){
    if(t == NULL) return;
    if(t->tag == 1){
        printf("At NT : %s\n", nonTerminalMap[t->type.nonterminal]);
        getchar();
    }
    else{
        printf("At T : %s\n", terminalMap[t->type.terminal]);
        getchar();
    }
}












/////////////////////////////////////////////////////////////////////////////


char *pTypeMap [] = {
    "integer",
    "real",
    "boolean"
};

void printTypeExpression(typeExpression typeExpr, int type) {
    if(type == 0) { // primitive
        printf("<type=%s>\n", pTypeMap[typeExpr.primitive]);
    }
    else if(type == 1) { // rectangular
        printf("<type=rectangularArray, ");
        for(int i = 0; i < typeExpr.rectArray.dimensions; ++i) {     
            printf("range_R%d = ", i+1);
            printf("(%d, %d), ", typeExpr.rectArray.range1[i][0], typeExpr.rectArray.range1[i][1]);
        }
        printf("basicElementType = integer>\n");
    } else { // jagged
        printf("<type=jaggedArray, ");
        if(typeExpr.jaggedArray.dimensions == 2) { // 2D
            printf("dimensions=2, ");
            printf("range_R1=(%d, %d), ", typeExpr.jaggedArray.range1[0], typeExpr.jaggedArray.range1[1]);
            printf("range_R2 = (");
            int r2r = typeExpr.jaggedArray.range1[1] - typeExpr.jaggedArray.range1[0] + 1;
            for(int i = 0; i < r2r - 1; ++i) {
                printf("%d, ", typeExpr.jaggedArray.range2_2D[i]);
            }
            printf("%d), ", typeExpr.jaggedArray.range2_2D[r2r - 1]);
        } else { // 3D
            printf("dimensions=3, ");
            printf("range_R1=(%d, %d), ", typeExpr.jaggedArray.range1[0], typeExpr.jaggedArray.range1[1]);
            printf("range_R2 = (");
            int r2r = typeExpr.jaggedArray.range1[1] - typeExpr.jaggedArray.range1[0] + 1;
            for(int i = 0; i < r2r - 1; ++i) {
                printf("%d[", typeExpr.jaggedArray.range2_2D[i]);
                for(int j = 0; j < typeExpr.jaggedArray.range2_2D[i] - 1; ++j) {
                    printf("%d, ", typeExpr.jaggedArray.range2_3D[i][j]);
                }
                printf("%d], ", typeExpr.jaggedArray.range2_3D[i][typeExpr.jaggedArray.range2_2D[i] - 1]);
            }
            printf("%d[", typeExpr.jaggedArray.range2_2D[r2r - 1]);
            for(int j = 0; j < typeExpr.jaggedArray.range2_2D[r2r - 1] - 1; ++j) {
                    printf("%d, ", typeExpr.jaggedArray.range2_3D[r2r - 1][j]);
                }
                printf("%d]), ", typeExpr.jaggedArray.range2_3D[r2r - 1][typeExpr.jaggedArray.range2_2D[r2r - 1] - 1]);
        } 

        printf("basicElementType = integer>\n");
    }
}

void printTypeExpressionTable(tableEntry** T) {
    for(int i = 0; i < HASH_DIM; i++){
        if(T[i] != NULL) {
            tableEntry* t = T[i];
            while(t != NULL){
                printf("\n%10s| %17s| %14s| ", t->name, typeMap[t->type], t->arrayType);
                // type expression
                printTypeExpression(t->typeExpr, t->type);
                t = t->next;
            }
        }
    }
}   
























