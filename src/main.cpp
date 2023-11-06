#include <stdio.h>
#include <stdlib.h>

#include "dedlist.h"

int main()
{
    Dedlist dedlist = {};

    dedlist_ctor(&dedlist, 10);

    size_t x_anchor_1 = -1;
    dedlist_push_head(&dedlist, 19, &x_anchor_1);
    printf("x_anchor_1 : <%lld>\n", x_anchor_1);

    size_t x_anchor_2 = -1;
    dedlist_push_head(&dedlist, 23, &x_anchor_2);
    printf("x_anchor_2 : <%lld>\n", x_anchor_2);

    size_t x_anchor_3 = -1;
    dedlist_insert(&dedlist, x_anchor_2, 42, &x_anchor_3);
    printf("x_anchor_3 : <%lld>\n", x_anchor_3);

    size_t x_anchor_4 = -1;
    dedlist_push_head(&dedlist, 17, &x_anchor_4);
    printf("x_anchor_4 : <%lld>\n", x_anchor_4);

    dedlist_delete_head(&dedlist);

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_dtor(&dedlist);

    return 0;
}
