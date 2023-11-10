#include "utils.h"

#include <assert.h>

size_t str_insert(  char *str_dest_begin,
                    const char *str_source,
                    size_t start_ind,
                    int *res )
{
    assert(str_dest_begin);
    assert(res);
    assert(str_source);
    assert(start_ind);

    size_t cnt = 0;
    int source_copied = 0;
    while ( str_dest_begin[cnt + start_ind] != '\0' )
    {
        str_dest_begin[cnt + start_ind] = str_source[cnt];

        cnt++;

        if ( str_source[cnt] == '\0' )
        {
            source_copied = 1;
            break;
        }

    }

    if ( source_copied )
        *res = 1;
    else
        *res = 0;

    return cnt + start_ind;
}
