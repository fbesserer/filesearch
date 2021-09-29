#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>

#define TABLE_SIZE 40

typedef struct {
    unsigned short marker;
    bool found; 
} markers;

int init_hashtable(markers *ht[]);
void print_hashtable(markers *ht[]);
int hash(unsigned short *marker);
unsigned short in_hashtable(markers *ht[], unsigned short search);
void fill_hashtable(markers *ht[]);

#endif
