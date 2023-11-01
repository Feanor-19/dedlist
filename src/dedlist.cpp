#include <assert.h>

#include "dedlist.h"

void print_dedlist_status_code_message( DedlistStatusCode code, FILE *stream)
{
    fprintf(stream, dedlist_status_code_messages[code]);
}

void inline init_zeroth_elem_( VoidDedlist *dedlist_ptr )
{
    assert(dedlist_ptr);

    dedlist_ptr->nodes[0].data_ptr = NULL;
    dedlist_ptr->nodes[0].next = 0;
    dedlist_ptr->nodes[0].prev = 0;
}

void inline init_free_elem( VoidDedlist *dedlist_ptr, ptrdiff_t free_elem_ind )
{
    assert(dedlist_ptr);

    dedlist_ptr->nodes[free_elem_ind].data_ptr = NULL;
    dedlist_ptr->nodes[free_elem_ind].prev = -1;
    dedlist_ptr->nodes[free_elem_ind].next = dedlist_ptr->free;

    dedlist_ptr->free = free_elem_ind;
}

void inline init_new_free_elems_( VoidDedlist *dedlist_ptr, ptrdiff_t start_with )
{
    assert( dedlist_ptr );

    for ( ptrdiff_t ind = start_with; ind < dedlist_ptr->capacity; ind++ )
    {
        init_free_elem(dedlist_ptr, ind);
    }
}

DedlistStatusCode dedlist_ctor_( VoidDedlist *dedlist_ptr, size_t default_size
#ifdef DEDLIST_DO_DUMP
                                , DedlistOrigInfo orig_info
#endif
                                )
{
    assert( dedlist_ptr );
    assert(default_size > 0);

    dedlist_ptr->nodes = (DedlistNode *) calloc( default_size + 1, sizeof(DedlistNode) );
    if ( !dedlist_ptr->nodes )
        return DEDLIST_STATUS_ERROR_MEM_ALLOC;

    dedlist_ptr->capacity = default_size + 1;

    init_zeroth_elem_( dedlist_ptr );

    dedlist_ptr->free = 0;

    init_new_free_elems_(dedlist_ptr, 1);

#ifdef DEDLIST_DO_DUMP
    dedlist_ptr->orig_info = orig_info;
#endif

    return DEDLIST_STATUS_OK;
}

DedlistStatusCode dedlist_dtor( VoidDedlist *dedlist_ptr )
{
    if (dedlist_ptr->nodes)
        free(dedlist_ptr->nodes);

    dedlist_ptr->capacity = 0;
    dedlist_ptr->free = 0;

#ifdef DEDLIST_DO_DUMP
    dedlist_ptr->orig_info = {};
#endif

    return DEDLIST_STATUS_OK;
}

#ifdef DEDLIST_DO_DUMP

inline int verify_check_occupied_elems( VoidDedlist *dedlist_ptr )
{
    ptrdiff_t curr_ind = dedlist_get_head_ind( dedlist_ptr );
    size_t iterations = 0;
    while ( curr_ind != 0 )
    {
        if ( dedlist_ptr->nodes[curr_ind].prev != 0
            && dedlist_ptr->nodes[ dedlist_ptr->nodes[curr_ind].prev ].next != curr_ind )
        {
            return 0;
        }

        if ( dedlist_ptr->nodes[curr_ind].next != 0
            && dedlist_ptr->nodes[ dedlist_ptr->nodes[curr_ind].next ].prev != curr_ind )
        {
            return 0;
        }

        curr_ind = dedlist_ptr->nodes[curr_ind].next;
        iterations++;
        if (iterations > dedlist_ptr->capacity)
            return 0;
    }

    return 1;
}

inline int verify_check_free_elems( VoidDedlist *dedlist_ptr )
{
    ptrdiff_t curr_ind = dedlist_ptr->free;
    size_t iterations = 0;
    while ( curr_ind != 0 )
    {
        if ( dedlist_ptr->nodes[curr_ind].prev != -1 )
        {
            return 0;
        }

        curr_ind = dedlist_ptr->nodes[curr_ind].next;
        iterations++;
        if (iterations > dedlist_ptr->capacity)
            return 0;
    }

    return 1;
}

#define DEF_VERIFY_FLAG(bit, name, message, cond) if ((cond)) {verify_res |= (1 << (bit));}
dl_verify_res_t dedlist_verify( VoidDedlist *dedlist_ptr )
{
    dl_verify_res_t verify_res = 0;

    #include "dedlist_verify_flags.h"

    return verify_res;
}
#undef DEF_VERIFY_FLAG

void dedlist_dump_( VoidDedlist *dedlist_ptr,
                    dl_verify_res_t verify_res,
                    const char *file,
                    const int line,
                    const char *func )
{



}

void dedlist_print_verify_res_(FILE *stream, int verify_res)
{
    fprintf(stream, "Dedlist verification result: <%d>\n", verify_res);
    for (size_t ind = 0; ind < sizeof(dedlist_verification_messages)/sizeof(dedlist_verification_messages[0]); ind++)
    {
        if (verify_res & ( 1 << ind ))
        {
            printf("----> %s\n", dedlist_verification_messages[ind]);
        }
    }
}

#endif //DEDLIST_DO_DUMP

ptrdiff_t dedlist_get_head_ind( VoidDedlist *dedlist_ptr )
{
    assert(dedlist_ptr);

    return dedlist_ptr->nodes[0].next;
}

ptrdiff_t dedlist_get_tail_ind( VoidDedlist *dedlist_ptr )
{
    assert(dedlist_ptr);

    return dedlist_ptr->nodes[0].prev;
}
