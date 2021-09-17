#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

/* Hashtable content
0: ffc0
1: ffd9
2: ffda
3: ffdb
4: ffc4
5: ---
*/

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

void fill_hashtable(markers *ht[]) {
    // initialises and fills the hashtable with the jpg markers, static so that information is not lost when function is exited
    init_hashtable(ht);
    static markers qt =  {.marker = 0xffdb};
    static markers sof = {.marker = 0xffc0};
    static markers huf = {.marker = 0xffc4};
    static markers sod = {.marker = 0xffda};
    static markers eoi = {.marker = 0xffd9};
    ht[hash(&qt.marker)]= &qt;
    ht[hash(&sof.marker)]= &sof;
    ht[hash(&huf.marker)]= &huf;
    ht[hash(&sod.marker)]= &sod;
    ht[hash(&eoi.marker)]= &eoi;
}

int in_hashtable(markers *ht[], unsigned short search) {
    // returns bucket if bytes are present inside 
    int bucket = hash(&search);
    if (ht[bucket] != NULL && ht[bucket]->marker == search) {
        return bucket;    
    }
    return -1;
}

