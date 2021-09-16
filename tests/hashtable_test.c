#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../src/hashtable.h"

Test(hashtable_test, isinhashtable) {
    
    markers *hashtable[6];
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
    cr_assert(strlen("") == 0);
}
