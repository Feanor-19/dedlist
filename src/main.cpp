#include <stdio.h>
#include <stdlib.h>

#include "dedlist.h"
#include "utils.h"

int main()
{
    char str1[20] = "hello world";
    const char *str2 = "fip";
    int res = -1;
    size_t ind = str_insert(str1, str2, 10, &res);
    printf("%d, %lld, <%s>\n", res, ind, str1);

    /*
    Dedlist dedlist = {};

    dedlist_ctor(&dedlist, 2);

    for (int x = 1; x <= 19; x++)
    {
        dedlist_push_tail( &dedlist, x );
    }

    DEDLIST_DUMP(&dedlist, 0);

    for (int x = 1; x <= 19; x++)
    {
        dedlist_delete_tail( &dedlist );
    }

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_push_head(&dedlist, 42);

    DEDLIST_DUMP(&dedlist, 0);

    dedlist_dtor(&dedlist);
    */

    return 0;
}
