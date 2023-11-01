
// DSL
#define _DEDLIST_PTR (dedlist_ptr)
#define _IS_PTRDIFF_TOO_BIG(x_) ( _x >= PTRDIFF_MAX - 10 )
#define _CHECK_OCCUPIED_ELEMS() verify_check_occupied_elems(_DEDLIST_PTR)
#define _CHECK_FREE_ELEMS() verify_check_free_elems(_DEDLIST_PTR)

//------------------------------------------------------------------------------------------------------

DEF_VERIFY_FLAG(    0,
                    NULL_DEDLIST_PNT,
                    "NULL_DEDLIST_PNT",
                    (!_DEDLIST_PTR) )

DEF_VERIFY_FLAG(    1,
                    NULL_NODES,
                    "NULL_NODES",
                    ( _DEDLIST_PTR && !_DEDLIST_PTR->nodes))

DEF_VERIFY_FLAG(    2,
                    ZERO_CAPACITY,
                    "ZERO_CAPACITY",
                    ( _DEDLIST_PTR && _DEDLIST_PTR->capacity == 0))

DEF_VERIFY_FLAG(    3,
                    TOO_BIG_CAPACITY,
                    "TOO_BIG_CAPACITY",
                    ( _DEDLIST_PTR && _IS_PTRDIFF_TOO_BIG(_DEDLIST_PTR->capacity) ))

DEF_VERIFY_FLAG(    4,
                    WRONG_FREE,
                    "WRONG_FREE",
                    ( _DEDLIST_PTR && (_DEDLIST_PTR->free < 0 || _IS_PTRDIFF_TOO_BIG(_DEDLIST_PTR->free) ) ))

DEF_VERIFY_FLAG(    5,
                    OCCUPIED_ELEMS_LINKS_ERROR,
                    "OCCUPIED_ELEMS_LINKS_ERROR",
                    ( _DEDLIST_PTR && !_CHECK_OCCUPIED_ELEMS() ))

DEF_VERIFY_FLAG(    6,
                    FREE_ELEMS_LINKS_ERROR,
                    "FREE_ELEMS_LINKS_ERROR",
                    ( _DEDLIST_PTR && !_CHECK_FREE_ELEMS() ))

//------------------------------------------------------------------------------------------------------

// UNDEF DSL
#undef _DEDLIST_PTR
#undef _IS_PTRDIFF_TOO_BIG
#undef _CHECK_OCCUPIED_ELEMS
#undef _CHECK_FREE_ELEMS
