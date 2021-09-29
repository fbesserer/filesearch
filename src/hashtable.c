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
        return (int) *marker % 39;
    }
}

void fill_hashtable(markers *ht[]) {
    // initialises and fills the hashtable with the jpg markers, static so that information is not lost when function is exited
    init_hashtable(ht);
    /*
    static markers qt =  {.marker = 0xffdb};
    static markers sof = {.marker = 0xffc0};
    static markers huf = {.marker = 0xffc4};
    static markers sod = {.marker = 0xffda};
    static markers eoi = {.marker = 0xffd9};
    ht[hash(&qt.marker)]= &qt;
    ht[hash(&sof.marker)]= &sof;
    ht[hash(&huf.marker)]= &huf;
    ht[hash(&sod.marker)]= &sod;
    ht[hash(&eoi.marker)]= &eoi;*/
    static markers qt =  {.marker = 0xffdb};
    static markers sof = {.marker = 0xffc0};
    static markers huf = {.marker = 0xffc4};
    static markers sod = {.marker = 0xffda};
    static markers eoi = {.marker = 0xffd9};
    static markers app0 =  {.marker = 0xffe0};
    static markers app1 = {.marker = 0xffe1};
    static markers app2 = {.marker = 0xffe2};
    static markers app3 = {.marker = 0xffe3};
    static markers app4 = {.marker = 0xffec};
    static markers app5 =  {.marker = 0xffed};
    static markers app6 = {.marker = 0xffee};

    ht[hash(&qt.marker)]= &qt;
    ht[hash(&sof.marker)]= &sof;
    ht[hash(&huf.marker)]= &huf;
    ht[hash(&sod.marker)]= &sod;
    ht[hash(&eoi.marker)]= &eoi;
    ht[hash(&app0.marker)]= &app0;
    ht[hash(&app1.marker)]= &app1;
    ht[hash(&app2.marker)]= &app2;
    ht[hash(&app3.marker)]= &app3;
    ht[hash(&app4.marker)]= &app4;
    ht[hash(&app5.marker)]= &app5;
    ht[hash(&app6.marker)]= &app6;
}

unsigned short in_hashtable(markers *ht[], unsigned short search) {
    // returns bucket if bytes are present inside 
    int bucket = hash(&search);
    if (ht[bucket] != NULL && ht[bucket]->marker == search) {
        return search;    
    }
    return 0xffff;
}

