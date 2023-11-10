#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"

#include "dedlist.h"

inline int is_anchor_valid_( Dedlist *dedlist_ptr, size_t anchor )
{
    if ( anchor < dedlist_ptr->capacity )
    {
        if ( !is_node_free_(dedlist_ptr, anchor) )
            return 1;
        else
            return 0;
    }

    return 0;
}

DedlistStatusCode dedlist_insert(   Dedlist *dedlist_ptr,
                                    size_t anchor,
                                    Elem_t value,
                                    size_t* inserted_elem_anchor_ptr)
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    if ( !is_anchor_valid_( dedlist_ptr, anchor ) )
        return DEDLIST_STATUS_ERROR_INVALID_ANCHOR;

    if (dedlist_ptr->free == 0)
    {
        DL_WRP_RET( dedlist_realloc_up_(dedlist_ptr) );
    }

    ptrdiff_t new_elem_ind = dedlist_ptr->free;

    dedlist_ptr->free = dedlist_ptr->nodes[new_elem_ind].next;

    dedlist_ptr->nodes[new_elem_ind].data = value;


    ptrdiff_t tmp = dedlist_ptr->nodes[anchor].next;
    dedlist_ptr->nodes[anchor].next = new_elem_ind;
    dedlist_ptr->nodes[new_elem_ind].next = tmp;
    dedlist_ptr->nodes[tmp].prev = new_elem_ind;

    dedlist_ptr->nodes[new_elem_ind].prev = anchor;

    if (inserted_elem_anchor_ptr != NULL)
        *inserted_elem_anchor_ptr = (size_t) new_elem_ind;
    dedlist_ptr->size++;

    return DEDLIST_STATUS_OK;
}

DedlistStatusCode dedlist_delete(   Dedlist *dedlist_ptr,
                                    size_t anchor )
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    ptrdiff_t saved_prev = dedlist_ptr->nodes[anchor].prev;
    ptrdiff_t saved_next = dedlist_ptr->nodes[anchor].next;

    init_free_elem_(dedlist_ptr, anchor);

    dedlist_ptr->nodes[saved_prev].next = saved_next;
    dedlist_ptr->nodes[saved_next].prev = saved_prev;

    dedlist_ptr->size--;

    return DEDLIST_STATUS_OK;
}

DedlistStatusCode dedlist_get_by_anchor( Dedlist *dedlist_ptr, size_t anchor, Elem_t *ret)
{
    DEDLIST_SELFCHECK(dedlist_ptr);
    assert(ret);

    if (!is_anchor_valid_(dedlist_ptr, anchor))
        return DEDLIST_STATUS_ERROR_INVALID_ANCHOR;

    *ret = dedlist_ptr->nodes[anchor].data;

    return DEDLIST_STATUS_OK;
}

DedlistStatusCode dedlist_get_head( Dedlist *dedlist_ptr, Elem_t *ret)
{
    return dedlist_get_by_anchor( dedlist_ptr, dedlist_get_head_ind(dedlist_ptr), ret );
}

DedlistStatusCode dedlist_get_tail( Dedlist *dedlist_ptr, Elem_t *ret)
{
    return dedlist_get_by_anchor( dedlist_ptr, dedlist_get_tail_ind(dedlist_ptr), ret );
}

DedlistStatusCode dedlist_push_head(    Dedlist *dedlist_ptr,
                                        Elem_t value,
                                        size_t* inserted_elem_anchor_ptr )
{
    return dedlist_insert( dedlist_ptr, 0, value, inserted_elem_anchor_ptr );
}

DedlistStatusCode dedlist_push_tail(    Dedlist *dedlist_ptr,
                                        Elem_t value,
                                        size_t* inserted_elem_anchor_ptr )
{
    return dedlist_insert( dedlist_ptr, dedlist_get_tail_ind(dedlist_ptr), value, inserted_elem_anchor_ptr );
}

DedlistStatusCode dedlist_delete_head( Dedlist *dedlist_ptr )
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    return dedlist_delete(dedlist_ptr, dedlist_get_head_ind(dedlist_ptr));
}

DedlistStatusCode dedlist_delete_tail( Dedlist *dedlist_ptr )
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    return dedlist_delete(dedlist_ptr, dedlist_get_tail_ind(dedlist_ptr));
}

size_t dedlist_get_head_ind( Dedlist *dedlist_ptr )
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    return (size_t) dedlist_ptr->nodes[0].next;
}

size_t dedlist_get_tail_ind( Dedlist *dedlist_ptr )
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    return (size_t) dedlist_ptr->nodes[0].prev;
}

void dedlist_print_status_code_message( DedlistStatusCode code, FILE *stream)
{
    fprintf(stream, "%s", dedlist_status_code_messages[code]);
}

void init_zeroth_elem_( Dedlist *dedlist_ptr )
{
    assert(dedlist_ptr);

    dedlist_ptr->nodes[0].data = ELEM_T_DEFAULT_VALUE;
    dedlist_ptr->nodes[0].next = 0;
    dedlist_ptr->nodes[0].prev = 0;
}

void init_free_elem_( Dedlist *dedlist_ptr, ptrdiff_t free_elem_ind )
{
    assert(dedlist_ptr);

    dedlist_ptr->nodes[free_elem_ind].data = ELEM_T_DEFAULT_VALUE;
    dedlist_ptr->nodes[free_elem_ind].prev = -1;
    dedlist_ptr->nodes[free_elem_ind].next = dedlist_ptr->free;

    dedlist_ptr->free = free_elem_ind;
}

void init_new_free_elems_( Dedlist *dedlist_ptr, ptrdiff_t start_with )
{
    assert( dedlist_ptr );

    for ( ptrdiff_t ind = start_with; ind < (ptrdiff_t) dedlist_ptr->capacity; ind++ )
    {
        init_free_elem_(dedlist_ptr, ind);
    }
}

DedlistStatusCode dedlist_ctor_( Dedlist *dedlist_ptr, size_t default_size
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

    dedlist_ptr->free = 0; // изменится после иницииализации новых пустых элементов
    dedlist_ptr->size = 0;

    init_new_free_elems_(dedlist_ptr, 1);

#ifdef DEDLIST_DO_DUMP
    dedlist_ptr->orig_info = orig_info;
#endif

    return DEDLIST_STATUS_OK;
}

DedlistStatusCode dedlist_realloc_up_( Dedlist *dedlist_ptr )
{
    DEDLIST_SELFCHECK(dedlist_ptr);

    const size_t MEM_MULTIPLIER = 2;

    size_t old_capacity = dedlist_ptr->capacity;
    DedlistNode *new_mem = (DedlistNode*) realloc( dedlist_ptr->nodes, MEM_MULTIPLIER*old_capacity*sizeof(DedlistNode));
    if (!new_mem)
        return DEDLIST_STATUS_ERROR_MEM_ALLOC;

    dedlist_ptr->nodes = new_mem;

    dedlist_ptr->capacity = MEM_MULTIPLIER * old_capacity;

    init_new_free_elems_(dedlist_ptr, old_capacity);

    return DEDLIST_STATUS_OK;
}

DedlistStatusCode dedlist_shrink_to_fit_and_loose_anchors( Dedlist *dedlist_ptr )
{

}

DedlistStatusCode dedlist_dtor( Dedlist *dedlist_ptr )
{
    if (dedlist_ptr->nodes)
        free(dedlist_ptr->nodes);

    dedlist_ptr->nodes = NULL;

    dedlist_ptr->capacity = 0;
    dedlist_ptr->free = 0;
    dedlist_ptr->size = 0;

#ifdef DEDLIST_DO_DUMP
    dedlist_ptr->orig_info = {};
#endif

    return DEDLIST_STATUS_OK;
}

//---------------------------------------DEDLIST_DO_DUMP----------------------------------------

#ifdef DEDLIST_DO_DUMP

inline int verify_check_occupied_elems( Dedlist *dedlist_ptr )
{
    ptrdiff_t curr_ind = dedlist_ptr->nodes[0].next;
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

inline int verify_check_free_elems( Dedlist *dedlist_ptr )
{
    ptrdiff_t curr_ind = dedlist_ptr->free;
    size_t iterations = 0;
    while ( curr_ind != 0 )
    {
        if ( !is_node_free_( dedlist_ptr, curr_ind ) )
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
dl_verify_res_t dedlist_verify( Dedlist *dedlist_ptr )
{
    dl_verify_res_t verify_res = 0;

    #include "dedlist_verify_flags.h"

    return verify_res;
}
#undef DEF_VERIFY_FLAG

//! @details Prints current local time into string stream.
//! Is string's capacity is exceeded, returns 0. If ok, returns 1.
inline int get_as_str_curr_local_time_(char *stream, size_t capacity )
{
    time_t curr_time = time(NULL);
    tm curr_local_time = *localtime(&curr_time);

    int cnt = snprintf( stream, capacity, "%d-%02d-%02d_%02d-%02d-%02d",
                        curr_local_time.tm_year + 1900,
                        curr_local_time.tm_mon + 1,
                        curr_local_time.tm_mday,
                        curr_local_time.tm_hour,
                        curr_local_time.tm_min,
                        curr_local_time.tm_sec);

    if (cnt > 0 && cnt < (int) capacity)
        return 1;
    else
        return 0;
}

inline DedlistStatusCode create_dump_folder_( char **curr_dump_dir_ptr )
{
    assert(curr_dump_dir_ptr);

    char *dir_name = (char*) calloc(DEDLIST_MAX_DUMP_PATH_LENGHT, sizeof(char));

    int res = 1;
    size_t curr_dir_name_ind = str_insert(  dir_name,
                                            DEDLIST_MAX_DUMP_PATH_LENGHT,
                                            DEDLIST_DUMP_PATH,
                                            0,
                                            &res);
    if ( !res )
        return DEDLIST_STATUS_ERROR_MAX_DUMP_PATH_LEN_TOO_SHORT;

    char curr_time[DEDLIST_MAX_DUMP_PATH_LENGHT] = "";
    if ( !get_as_str_curr_local_time_(curr_time, DEDLIST_MAX_DUMP_PATH_LENGHT) )
    {
        assert(0 && "Too short string for curr_local_time! Please chahge it!");
    }

    curr_dir_name_ind = str_insert( dir_name,
                                    DEDLIST_MAX_DUMP_PATH_LENGHT,
                                    curr_time,
                                    curr_dir_name_ind,
                                    &res);
    if ( !res )
        return DEDLIST_STATUS_ERROR_MAX_DUMP_PATH_LEN_TOO_SHORT;


    curr_dir_name_ind = str_insert( dir_name,
                                    DEDLIST_MAX_DUMP_PATH_LENGHT,
                                    "\\",
                                    curr_dir_name_ind,
                                    &res);
    if ( !res )
        return DEDLIST_STATUS_ERROR_MAX_DUMP_PATH_LEN_TOO_SHORT;

    struct stat st = {};

    if (stat(DEDLIST_DUMP_PATH, &st) == -1) {
        int res_dir = mkdir(DEDLIST_DUMP_PATH);

        if (res_dir != 0)
            return DEDLIST_STATUS_ERROR_CANT_CREATE_DUMP_DIR;
    }

    if (stat(dir_name, &st) == -1) {
        int res_dir = mkdir(dir_name);

        if (res_dir != 0)
            return DEDLIST_STATUS_ERROR_CANT_CREATE_DUMP_DIR;
    }

    *curr_dump_dir_ptr = dir_name;

    return DEDLIST_STATUS_OK;
}

//! @brief Pointer to newly created file is stored in *ret_ptr
inline DedlistStatusCode create_tmp_dot_file_( const char *dot_file_path, FILE **ret_ptr )
{
    assert(dot_file_path);
    assert(ret_ptr);

    *ret_ptr = fopen( dot_file_path, "w" );
    if ( !(*ret_ptr) )
        return DEDLIST_STATUS_ERROR_CANT_OPEN_DUMP_FILE;

    return DEDLIST_STATUS_OK;
}

// TODO - в другой файл? что делать с такой большой функцией?
// если в другой файл, тогда dedlist будет уже из более чем двух файлов,
// а это неудобно подключать... можно попробовать разобраться с библиотеками
inline DedlistStatusCode write_dot_file_for_dump_(  FILE *dot_file,
                                                    Dedlist *dedlist_ptr,
                                                    dl_verify_res_t verify_res,
                                                    const char *called_from_file,
                                                    const int called_from_line,
                                                    const char *called_from_func  )
{
    // писать сразу в файл медленно, но когда генерится дамп, эта скорость не важна

#define COLOR_BG "#2D4059"
#define COLOR_OCCUP_NODE_COLOR "#ECC237"
#define COLOR_OCCUP_NODE_FILL "#EA5455"
#define COLOR_FREE_NODE_COLOR "#ECC237"
#define COLOR_FREE_NODE_FILL "#8ccb5e"
#define COLOR_LABEL_COLOR "#EA5455"
#define COLOR_LABEL_FILL "#ECC237"
#define COLOR_EDGE_PREV "#F07B3F"
#define COLOR_EDGE_NEXT "#FFD460"
#define COLOR_EDGE_FREE "#8ccb5e"

    // splines = ortho делает стрелочки с прямыми углами, но это очень плохо работает
    // с выстроенными в ряд узлами

    //---------------------------------------------------------------------
    fprintf(dot_file,   "digraph{\n"
                        "splines=ortho;\n"
                        "bgcolor=\"" COLOR_BG "\";"
                        "\n\n\n");
    //---------------------------------------------------------------------

    //---------------------------------NODE_TEXT--------------------------
    fprintf(dot_file,   "NODE_TEXT[shape=note, fontname=\"verdana\",\n"
                        "style=bold, style=filled,\n"
                        "color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\",\n"
                        "label = \"");
    fprintf(dot_file,   "Dedlist[%p] (%s) declared in %s(%d), in function %s.\\n"
                        "DEDLIST_DUMP() called from %s(%d), from function %s.\\n"
                        "capacity: %lld; free: %lld; nodes: [%p].",
                        dedlist_ptr,
                        dedlist_ptr->orig_info.dedlist_name,
                        dedlist_ptr->orig_info.dedlist_orig_file_name,
                        dedlist_ptr->orig_info.orig_line,
                        dedlist_ptr->orig_info.orig_func_name,
                        called_from_file,
                        called_from_line,
                        called_from_func,
                        dedlist_ptr->capacity,
                        dedlist_ptr->free,
                        dedlist_ptr->nodes);
    dedlist_print_verify_res_(dot_file, verify_res);
    fprintf(dot_file, "\"]\n\n\n");
    //---------------------------------------------------------------------


    //------------------------------NODE_0---------------------------------
    fprintf(dot_file,   "NODE_0[shape=\"record\", fontname=\"verdana\",\n"
                        "style=bold, style=filled,\n"
                        "color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\",\n"
                        "label = <<table cellspacing=\"0\">\n"
                        "<tr><td colspan=\"2\">ind: 0</td></tr>\n"
                        "<tr><td colspan=\"2\">data: FICTIONAL</td></tr>\n"
                        "<tr><td>prev: %td</td><td>next: %td</td></tr></table>>];\n\n\n",
                        dedlist_ptr->nodes[0].prev,
                        dedlist_ptr->nodes[0].next);
    //---------------------------------------------------------------------


    //---------------------------OTHER_NODES-------------------------------
    for (size_t ind = 1; ind < dedlist_ptr->capacity; ind++)
    {
        if ( !is_node_free_(dedlist_ptr, ind) )
        {
            // occupied node
            fprintf(dot_file,   "NODE_%llu[shape=\"record\", fontname=\"verdana\",\n"
                                "style=bold, style=filled,\n"
                                "color=\"" COLOR_OCCUP_NODE_COLOR "\", fillcolor=\"" COLOR_OCCUP_NODE_FILL "\",\n"
                                "label = <<table cellspacing=\"0\">\n"
                                "<tr><td colspan=\"2\">ind: %llu</td></tr>\n"
                                "<tr><td colspan=\"2\">data: ",
                                ind,
                                ind);
            dedlist_print_elem_t(dot_file, dedlist_ptr->nodes[ind].data );
            fprintf(dot_file,   "</td></tr>\n"
                                "<tr><td>prev: %td</td><td>next: %td</td></tr></table>>];\n\n",
                                dedlist_ptr->nodes[ind].prev,
                                dedlist_ptr->nodes[ind].next);
        }
        else
        {
            //free node
            fprintf(dot_file,   "NODE_%llu[shape=\"record\", fontname=\"verdana\",\n"
                                "style=bold, style=filled,\n"
                                "color=\"" COLOR_FREE_NODE_COLOR "\", fillcolor=\"" COLOR_FREE_NODE_FILL "\",\n"
                                "label = <<table cellspacing=\"0\">\n"
                                "<tr><td colspan=\"2\">ind: %llu</td></tr>\n"
                                "<tr><td colspan=\"2\">data: free</td></tr>\n"
                                "<tr><td>prev: %td</td><td>next: %td</td></tr></table>>];\n\n",
                                ind,
                                ind,
                                dedlist_ptr->nodes[ind].prev,
                                dedlist_ptr->nodes[ind].next);
        }
    }
    //---------------------------------------------------------------------


    //-------------------------------SPECIAL_LABELS------------------------
    fprintf(dot_file,   "HEAD[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n"
                        "TAIL[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n"
                        "FREE[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n\n");
    //---------------------------------------------------------------------


    //-----------------------------------NODES_SEWING----------------------
    fprintf(dot_file, "{rank=same; NODE_TEXT ");
    for (size_t ind = 0; ind < dedlist_ptr->capacity; ind++)
    {
        fprintf(dot_file,   "NODE_%lld ", ind);
    }
    fprintf(dot_file,   "}\n");

    fprintf(dot_file,   "NODE_TEXT->NODE_0[weight=10, style=invis];\n");
    for (size_t ind = 0; ind < dedlist_ptr->capacity - 1; ind++)
    {
        fprintf(dot_file,   "NODE_%lld->NODE_%lld[weight=10, style=invis];\n",
                            ind, ind+1);
    }
    //---------------------------------------------------------------------


    //----------------------------SPECIAL_LABELS_EDGES---------------------
    fprintf(dot_file,   "HEAD->NODE_%td[style=invis];\n"
                        "TAIL->NODE_%td[style=invis];\n"
                        "FREE->NODE_%td[style=invis];\n\n\n",
                        dedlist_get_head_ind(dedlist_ptr),
                        dedlist_get_tail_ind(dedlist_ptr),
                        dedlist_ptr->free );
    //---------------------------------------------------------------------


    //--------------------------------------EDGES--------------------------
    for (size_t ind = 0; ind < dedlist_ptr->capacity; ind++)
    {
        if ( !is_node_free_(dedlist_ptr, ind) )
        {
            // occupied node
            fprintf(dot_file,   "NODE_%lld->NODE_%td[color=\"" COLOR_EDGE_PREV "\", penwidth=2];\n"
                                "NODE_%lld->NODE_%td[color=\"" COLOR_EDGE_NEXT "\", penwidth=2];\n\n",
                                ind,
                                dedlist_ptr->nodes[ind].prev,
                                ind,
                                dedlist_ptr->nodes[ind].next);
        }
        else
        {
            // free node
            fprintf(dot_file,   "NODE_%lld->NODE_%lld[color=\"" COLOR_EDGE_FREE "\", penwidth=2];\n",
                                ind,
                                dedlist_ptr->nodes[ind].next);
        }
    }
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    fprintf(dot_file, "\n}\n");
    //---------------------------------------------------------------------

#undef COLOR_BG
#undef COLOR_OCCUP_NODE_COLOR
#undef COLOR_OCCUP_NODE_FILL
#undef COLOR_FREE_NODE_COLOR
#undef COLOR_FREE_NODE_FILL
#undef COLOR_LABEL_COLOR
#undef COLOR_LABEL_FILL
#undef COLOR_EDGE_PREV
#undef COLOR_EDGE_NEXT
#undef COLOR_EDGE_FREE

    return DEDLIST_STATUS_OK;
}

inline DedlistStatusCode generate_dump_img_( const char * dump_dot_path, const char * dump_img_path )
{
    char cmd[DEDLIST_MAX_CMD_GEN_DUMP_IMG_LENGHT] = {};
    int written_chars = snprintf(   cmd,
                                    DEDLIST_MAX_CMD_GEN_DUMP_IMG_LENGHT,
                                    "dot %s -Tjpg -o %s",
                                    dump_dot_path,
                                    dump_img_path);

    if ( written_chars >= (int) DEDLIST_MAX_CMD_GEN_DUMP_IMG_LENGHT)
    {
        fprintf(stderr, "Command to generate dump image is too long, can't execute it. "
        "Please, make paths to files shorter.\n");
        return DEDLIST_STATUS_ERROR_TOO_LONG_CMD_GEN_DUMP_IMG;
    }

    system(cmd);

    fprintf(stderr, "Dedlist dump image is generated!\n");

    return DEDLIST_STATUS_OK;
}

#ifdef DEDLIST_SHOW_DUMP_IMG
inline DedlistStatusCode show_dump_img_( const char * dump_img_path )
{
    system(dump_img_path);

    return DEDLIST_STATUS_OK;
}
#endif

inline DedlistStatusCode free_dot_file_( FILE * dot_tmp_file )
{
    fclose(dot_tmp_file);

    return DEDLIST_STATUS_OK;
}

void dedlist_dump_( Dedlist *dedlist_ptr,
                    dl_verify_res_t verify_res,
                    const char *file,
                    const int line,
                    const char *func )
{

    FILE *dot_file = NULL;

    char *curr_dump_dir = NULL;
    DL_WRP_PRINT( create_dump_folder_(&curr_dump_dir) );

    char dot_file_path[DEDLIST_MAX_DUMP_PATH_LENGHT] = "";
    strcpy(dot_file_path, curr_dump_dir);
    strcat(dot_file_path, "dmp.dot");

    DL_WRP_PRINT( create_tmp_dot_file_( dot_file_path, &dot_file ) );

    DL_WRP_PRINT( write_dot_file_for_dump_(dot_file, dedlist_ptr, verify_res, file, line, func ) );

    DL_WRP_PRINT( free_dot_file_(dot_file) );

    char img_file_path[DEDLIST_MAX_DUMP_PATH_LENGHT] = "";
    strcpy(img_file_path, curr_dump_dir);
    strcat(img_file_path, "dmp.jpg");

    DL_WRP_PRINT( generate_dump_img_( dot_file_path, img_file_path ) );

#ifdef DEDLIST_SHOW_DUMP_IMG
    DL_WRP_PRINT( show_dump_img_( img_file_path ) );
#endif

    free(curr_dump_dir);

#ifdef DEDLIST_ABORT_ON_DUMP
    abort();
#endif
}

int is_node_free_( Dedlist *dedlist_ptr, size_t anchor )
{
    assert( dedlist_ptr );

    return ( dedlist_ptr->nodes[anchor].prev == -1 );
}

void dedlist_print_verify_res_(FILE *stream, int verify_res)
{
    fprintf(stream, "Dedlist verification result: <%d>\n", verify_res);
    for (size_t ind = 0; ind < sizeof(dedlist_verification_messages)/sizeof(dedlist_verification_messages[0]); ind++)
    {
        if (verify_res & ( 1 << ind ))
        {
            fprintf(stream, "----> %s\n", dedlist_verification_messages[ind]);
        }
    }
}

#endif //DEDLIST_DO_DUMP

//---------------------------------------DEDLIST_DO_DUMP----------------------------------------
