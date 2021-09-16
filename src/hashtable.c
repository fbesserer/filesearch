#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

#define TABLE_SIZE 6
/*
typedef struct{
    unsigned short marker;
    bool found; 
    //char *marker; // wenn so, dann muss Speicher mit malloc allokiert wrden 
} markers;
*/
//markers *hashtable[TABLE_SIZE]; //array of ptrs to markers structs    

int init_hashtable(markers *ht[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht[i] = NULL;
    }
    return EXIT_SUCCESS;
}

void print_hashtable(markers *ht[]) {
    printf("Hashtable \n-----------------\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (ht[i] == NULL) {
            printf("%d: %s\n", i, "---");
        } else {
            printf("%d: %x\n", i, ht[i]->marker);
        }
    }
    printf("-----------------\n");
}

int hash(unsigned short *marker) {
    if (NULL == marker) {
        return -1;
    } else {
        return (int) *marker % 6;
    }
}

int in_hashtable(markers *ht[], unsigned short search) {
    int bucket = hash(&search);
    if (ht[bucket] != NULL && ht[bucket]->marker == search) {
        return bucket;    
    }
    return -1;
}

