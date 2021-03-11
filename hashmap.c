/********************************************
2017B4A70696P   Abdul Kadir Khimani
********************************************/

#include "hashmap.h"

int hashFunction(char *arr){
    int hashValue = 0;
    int len = strlen(arr);
    int i;
    for(i = 0; i < len; i++){
        hashValue += arr[i];
        hashValue %= HASH_DIM;
    }
    return hashValue;
}

/*This function creates and fills the hashtable*/
tableEntry** initialiseTable(){
    int i;
    tableEntry** T = (tableEntry**)malloc(sizeof(tableEntry*) * HASH_DIM);
    for(i = 0; i < HASH_DIM; i++)       //initializing all slots in hash table to NULL pointers
        T[i] = NULL;
    return T;
}

void addtoTable(tableEntry* t, tableEntry **T){
    int key = hashFunction(t->name);
    if(T[key] != NULL){
        tableEntry *temp = T[key];
        while(temp->next!=NULL)
            temp = temp->next;
        temp->next = t;
    }
    else
        T[key] = t;
    // printf("Table Entry Added\n");
}

/*This function looks up (char*)identifier in the (tableEntry**)hash_table. 
    Returns : NULL; if not keyword, 
              pointer to associated keyword's token; if keyword*/
tableEntry* lookup(char *identifier, tableEntry **T){
    int id_hash = hashFunction(identifier);
    tableEntry *temp = T[id_hash];
   
    while(temp != NULL){
        if(strcmp(identifier, temp->name) == 0)
            return temp;
        else
            temp = temp->next;
    }
    return NULL;
}

tableEntry* createTableEntry(int type, char *name, char *arrayType){
    tableEntry* t = (tableEntry*)malloc(sizeof(tableEntry));
    t->type = type;
    strcpy(t->name, name);
    strcpy(t->arrayType, arrayType);
    t->next = NULL;
    return t;
}
