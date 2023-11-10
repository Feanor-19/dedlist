#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

//! @details Inserts a copy of str_source into str_dest, starting
//! from index start_ind. If str_dest capacity is exceeded, str_source is partly copied
//! and res is set to 0. Otherwise res is set to 1.
//! @return Index of the char right after inserted string
//! (be careful, it might be index of the terminating symbol)
size_t str_insert(  char *str_dest_begin,
                    const char *str_source,
                    size_t start_ind,
                    int *res );

#endif /* UTILS_H */
