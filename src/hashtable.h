#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>

typedef struct {
    unsigned short marker;
    bool found; 
} markers;

int init_hashtable(markers *ht[]);
void print_hashtable(markers *ht[]);
int hash(unsigned short *marker);
int in_hashtable(markers *ht[], unsigned short search);

#endif
