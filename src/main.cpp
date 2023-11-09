#include <stdio.h>
#include <stdlib.h>

#include "dedlist.h"

int main()
{
    Dedlist dedlist = {};

    dedlist_ctor(&dedlist, 2);

    for (int x = 1; x <= 19; x++)
    {
        dedlist_push_tail( &dedlist, x );
    }

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_dtor(&dedlist);

    return 0;
}
