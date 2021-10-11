#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//#include "hashtable.h"

#define TABLE_SIZE 40

typedef struct {
    unsigned short marker;
    bool found; 
    //char *marker; // wenn so, dann muss Speicher mit malloc allokiert wrden 
} markers;

markers *hashtable[TABLE_SIZE]; //array of ptrs to markers structs    

int init_hashtable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashtable[i] = NULL;
    }
    return EXIT_SUCCESS;
}

void print_hashtable() {
    printf("Hashtable \n-----------------\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashtable[i] == NULL) {
            printf("%d: %s\n", i, "---");
        } else {
            printf("%d: %x\n", i, hashtable[i]->marker);
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

int in_hashtable(unsigned short search) {
    int bucket = hash(&search);
    if (hashtable[bucket] != NULL && hashtable[bucket]->marker == search) {
        return bucket;    
    }
    return -1;
}

int main (int argc, char *argv[]) {
    /*
    0xffdb: "Quantization Table",
    0xffc0: "Start of Frame",
    0xffc4: "Define Huffman Table",
    0xffda: "Start of Scan",
    0xffd9: "End of Image"*/
    
    init_hashtable();
    markers qt =  {.marker = 0xffdb};
    markers sof = {.marker = 0xffc0};
    markers huf = {.marker = 0xffc4};
    markers sod = {.marker = 0xffda};
    markers eoi = {.marker = 0xffd9};
    markers app0 =  {.marker = 0xffe0};
    markers app1 = {.marker = 0xffe1};
    markers app2 = {.marker = 0xffe2};
    markers app3 = {.marker = 0xffe3};
    markers app4 = {.marker = 0xffec};
    markers app5 =  {.marker = 0xffed};
    markers app6 = {.marker = 0xffee};

    hashtable[hash(&qt.marker)]= &qt;
    hashtable[hash(&sof.marker)]= &sof;
    hashtable[hash(&huf.marker)]= &huf;
    hashtable[hash(&sod.marker)]= &sod;
    hashtable[hash(&eoi.marker)]= &eoi;
    hashtable[hash(&app0.marker)]= &app0;
    hashtable[hash(&app1.marker)]= &app1;
    hashtable[hash(&app2.marker)]= &app2;
    hashtable[hash(&app3.marker)]= &app3;
    hashtable[hash(&app4.marker)]= &app4;
    hashtable[hash(&app5.marker)]= &app5;
    hashtable[hash(&app6.marker)]= &app6;

    
    print_hashtable();

    for (int i = 0x0000; i <= 0xffff; i++) {
        if (in_hashtable(i) >= 0) {
            printf("%x is in hashtable \n", i);
        }
    }
    

    return EXIT_SUCCESS;
}