#ifndef DEDLIST_H
#define DEDLIST_H

#include "stdio.h"
#include "stdint.h"
#include "limits.h"

/*
    AVAILABLE DEFINES:

    DEDLIST_DO_DUMP
*/
#define DEDLIST_DO_DUMP

typedef uint32_t dl_verify_res_t;

//------------------------------------------------------------------------------------------------------

#define DEF_STATUS_CODE(id, name, message) DEDLIST_STATUS_##name = id,
enum DedlistStatusCode
{
    #include "dedlist_status_codes.h"
};
#undef DEF_STATUS_CODE

#ifdef DEDLIST_DO_DUMP
#define DEF_VERIFY_FLAG(bit, name, message, cond) DEDLIST_VERIFY_##name = id,
enum DedlistVerifyResFlag
{
    #include "dedlist_verify_flags.h"
};
#undef DEF_VERIFY_FLAG
#endif // DEDLIST_DO_DUMP

struct DedlistNode
{
    void *data_ptr = NULL;
    ptrdiff_t next = 0;
    ptrdiff_t prev = -1;
};

#ifdef DEDLIST_DO_DUMP
struct DedlistOrigInfo
{
    const char *dedlist_name = NULL;
    const char *dedlist_orig_file_name = NULL;
    int orig_line = -1;
    const char *orig_func_name = NULL;
};
#endif // DEDLIST_DO_DUMP

struct VoidDedlist
{
    DedlistNode *nodes = NULL;
    size_t capacity = 0;

    //ptrdiff_t head = 0;   // head = next(0)
    //ptrdiff_t tail = 0;   // tail = prev(0)
    ptrdiff_t free = 0;

#ifdef DEDLIST_DO_DUMP
    DedlistOrigInfo orig_info = {};
#endif // DEDLIST_DO_DUMP
};

//------------------------------------------------------------------------------------------------------

#define DEF_STATUS_CODE(id, name, message) message,
const char *dedlist_status_code_messages[] =
{
    #include "dedlist_status_codes.h"
    "FICTIONAL_MESSAGE!"
};
#undef DEF_STATUS_CODE

#ifdef DEDLIST_DO_DUMP
#define DEF_VERIFY_FLAG(bit, name, message, cond) #bit ": " message,
const char *dedlist_verification_messages[] =
{
    #include "dedlist_verify_flags.h"
    "FICTIONAL_MESSAGE!"
};
#undef DEF_VERIFY_FLAG
#endif // DEDLIST_DO_DUMP

//------------------------------------------------------------------------------------------------------

DedlistStatusCode dedlist_ctor_( VoidDedlist *dedlist_ptr, size_t default_size
#ifdef DEDLIST_DO_DUMP
                                , DedlistOrigInfo orig_info
#endif
                                );

#ifdef DEDLIST_DO_DUMP

#define dedlist_ctor( dedlist_ptr, default_size ) dedlist_ctor_(    dedlist_ptr,            \
                                                                    default_size,           \
                                                                    {                       \
                                                                        #dedlist_ptr,       \
                                                                        __FILE__,           \
                                                                        __LINE__,           \
                                                                        __func__            \
                                                                    }                       \
                                                                )

#else // DEDLIST_DO_DUMP

#define dedlist_ctor(dedlist_ptr, default_size) dedlist_ctor_( dedlist_ptr, default_size )

#endif // DEDLIST_DO_DUMP

DedlistStatusCode dedlist_dtor( VoidDedlist *dedlist_ptr );

#ifdef DEDLIST_DO_DUMP
dl_verify_res_t dedlist_verify( VoidDedlist *dedlist_ptr );

void dedlist_dump_( VoidDedlist *dedlist_ptr,
                    dl_verify_res_t verify_res,
                    const char *file,
                    const int line,
                    const char *func );

#define DEDLIST_DUMP( dedlist_ptr, verify_res ) dedlist_dump_(  dedlist_ptr,        \
                                                                verify_res,         \
                                                                __FILE__,           \
                                                                __LINE__            \
                                                                __func__ )          \

#define DEDLIST_SELFCHECK( dedlist_ptr ) {                              \
    dl_verify_res_t verify_res = dedlist_verify( dedlist_ptr );         \
    if ( verify_res != 0 )                                              \
    {                                                                   \
        DEDLIST_DUMP(dedlist_ptr, verify_res);                          \
        return DEDLIST_STATUS_ERROR_VERIFY;                             \
    }                                                                   \
}

#else // DEDLIST_DO_DUMP

#define DEDLIST_DUMP( dedlist_ptr, verify_res ) ((void) 0)

#endif // DEDLIST_DO_DUMP

void print_dedlist_status_code_message( DedlistStatusCode code, FILE *stream);

ptrdiff_t dedlist_get_head_ind( VoidDedlist *dedlist_ptr );

ptrdiff_t dedlist_get_tail_ind( VoidDedlist *dedlist_ptr );

#endif
