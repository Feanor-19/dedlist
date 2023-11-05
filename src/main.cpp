#include <stdio.h>
#include <stdlib.h>

#include "dedlist.h"

// dot .\testgraph.dot -Tjpg -o .\testgraph.jpg

int main()
{
    Dedlist dedlist = {};

    dedlist_ctor(&dedlist, 5);

    size_t x_anchor_1 = -1;
    dedlist_push_head(&dedlist, 19, &x_anchor_1);
    printf("x_anchor : <%lld>\n", x_anchor_1);

    size_t x_anchor_2 = -1;
    dedlist_push_head(&dedlist, 23, &x_anchor_2);
    printf("x_anchor : <%lld>\n", x_anchor_2);

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_dtor(&dedlist);

    return 0;
}
