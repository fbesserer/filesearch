#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/hashtable.h"

markers *hashtable[6];
unsigned short ffc0 = 0xffc0;
unsigned short ffd9 = 0xffd9;
unsigned short ffda = 0xffda;
unsigned short ffdb = 0xffdb;
unsigned short ffc4 = 0xffc4;

void setup(void) {
    init_hashtable(hashtable);
    markers qt =  {.marker = 0xffdb};
    markers sof = {.marker = 0xffc0};
    markers huf = {.marker = 0xffc4};
    markers sod = {.marker = 0xffda};
    markers eoi = {.marker = 0xffd9};
    hashtable[hash(&qt.marker)]= &qt;
    hashtable[hash(&sof.marker)]= &sof;
    hashtable[hash(&huf.marker)]= &huf;
    hashtable[hash(&sod.marker)]= &sod;
    hashtable[hash(&eoi.marker)]= &eoi;
}

TestSuite(hashtable_test, .init=setup, .disabled=false);

const char descr[]="tests if only the jpg markers can be found in the hashtable";
Test(hashtable_test, isinhashtable, .description=descr) {
    for (int i = 0x0000; i <= 0xffbf; i++) {
        cr_expect(in_hashtable(hashtable, i) == -1, "%i shouldn't be in hashtable", i);
    }

    cr_expect(in_hashtable(hashtable, 0xffdb) != -1, "Expected 0xffdb in hashtable");
    cr_expect(in_hashtable(hashtable, 0xffc0) != -1, "Expected 0xffc0 in hashtable");
    cr_expect(in_hashtable(hashtable, 0xffc4) != -1, "Expected 0xffc4 in hashtable");
    cr_expect(in_hashtable(hashtable, 0xffda) != -1, "Expected 0xffda in hashtable");
    cr_expect(in_hashtable(hashtable, 0xffd9) != -1, "Expected 0xffd9 in hashtable");
    cr_expect(in_hashtable(hashtable, 0xffd7) == -1, "0xffd7 shouldn't be in hashtable");
    cr_expect(in_hashtable(hashtable, 0xffdc) == -1, "0xffdc shouldn't be in hashtable");
    cr_expect(in_hashtable(hashtable, 0x7368) == -1, "0x7368 shouldn't be in hashtable");
    cr_assert(strlen("") == 0);
}

const char descr1[] = "tests hash function";
Test(hashtable_test, hashfunction, .description=descr1) {
    cr_assert(hash(&ffc0) == 0, "Expected ffc0 to be at position 0");
    cr_assert(hash(&ffd9) == 1, "Expected ffd9 to be at position 1");
    cr_assert(hash(&ffda) == 2, "Expected ffda to be at position 2, but is: %d", hash(&ffda));
    cr_assert(hash(&ffdb) == 3, "Expected ffdb to be at position 3");
    cr_assert(hash(&ffc4) == 4, "Expected ffc4 to be at position 4");
}