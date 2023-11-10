#include <stdio.h>
#include <stdlib.h>

#include "dedlist.h"

int main()
{

    Dedlist dedlist = {};

    dedlist_ctor(&dedlist, 25);


    for (int x = 1; x <= 19; x++)
    {
        dedlist_push_tail( &dedlist, x );
    }

    DEDLIST_DUMP(&dedlist, 0);

    for (int x = 1; x <= 19; x++)
    {
        dedlist_delete_head( &dedlist );
    }

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_shrink_to_fit_and_loose_anchors( &dedlist );

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_push_head( &dedlist, 19 );

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_dtor(&dedlist);

    return 0;
}
