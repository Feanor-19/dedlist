#include <stdio.h>

#include "dedlist.h"

// dot .\testgraph.dot -Tjpg -o .\testgraph.jpg

int main()
{

    //printf("<\\n>\n");

    VoidDedlist dedlist = {};

    dedlist_ctor(&dedlist, 5);

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_dtor(&dedlist);

    return 0;
}
