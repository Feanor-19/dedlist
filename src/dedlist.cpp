#include <assert.h>


#include "dedlist.h"

void print_dedlist_status_code_message( DedlistStatusCode code, FILE *stream)
{
    fprintf(stream, "%s", dedlist_status_code_messages[code]);
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

    for ( ptrdiff_t ind = start_with; ind < (ptrdiff_t) dedlist_ptr->capacity; ind++ )
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

    dedlist_ptr->free = 0; // изменится после иницииализации новых пустых элементов

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

    dedlist_ptr->nodes = NULL;

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

//! @brief Pointer to newly created file is stored in *ret_ptr
inline DedlistStatusCode create_tmp_dot_file_( const char *dot_file_name, FILE **ret_ptr )
{
    *ret_ptr = fopen( dot_file_name, "w" );
    if ( !*ret_ptr )
        return DEDLIST_STATUS_ERROR_CANT_OPEN_DUMP_FILE;

    return DEDLIST_STATUS_OK;
}

// TODO - в другой файл? что делать с такой большой функцией?
inline DedlistStatusCode write_dot_file_for_dump_(  FILE *dot_file,
                                                    VoidDedlist *dedlist_ptr,
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

    //---------------------------------------------------------------------
    fprintf(dot_file,   "digraph{\n"
                        "rankdir=LR;\n"
                        "bgcolor=\"" COLOR_BG "\";\n\n\n");
    //---------------------------------------------------------------------

    //---------------------------------NODE_TEXT--------------------------
    fprintf(dot_file,   "NODE_TEXT[shape=note, fontname=\"verdana\",\n"
                        "style=bold, style=filled,\n"
                        "color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\",\n"
                        "label = \"");
    // TODO - добавить время дампа?
    fprintf(dot_file,   "DEDLIST DUMP:\\n"
                        "Dedlist[%p] (%s) declared in %s(%d), in function %s.\\n"
                        "STACK_DUMP() called from %s(%d), from function %s.\\n"
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
                        "color=\"" COLOR_OCCUP_NODE_COLOR "\", fillcolor=\"" COLOR_OCCUP_NODE_FILL "\",\n"
                        "label = \""
                        "{ <i>ind: 0 }|{ data:\\nNULL} | {<p>prev:\\n%td |<n>next:\\n%td}"
                        "\"];\n\n\n",
                        dedlist_ptr->nodes[0].prev,
                        dedlist_ptr->nodes[0].next);
    //---------------------------------------------------------------------


    //---------------------------OTHER_NODES-------------------------------
    for (size_t ind = 1; ind < dedlist_ptr->capacity; ind++)
    {
        if ( dedlist_ptr->nodes[ind].prev != -1 )
        {
            // occupied node
            // TODO - сделать печать любых типов, а не int
            fprintf(dot_file,   "NODE_%llu[shape=\"record\", fontname=\"verdana\",\n"
                                "style=bold, style=filled,\n"
                                "color=\"" COLOR_OCCUP_NODE_COLOR "\", fillcolor=\"" COLOR_OCCUP_NODE_FILL "\",\n"
                                "label = \""
                                "{ <i>ind: %llu }|{ data:\\n*[%p]=\\n%d} | {<p>prev:\\n%td |<n>next:\\n%td}"
                                "\"];\n\n",
                                ind,
                                ind,
                                dedlist_ptr->nodes + ind,
                                *( (int*)(dedlist_ptr->nodes + ind) ),
                                dedlist_ptr->nodes[ind].prev,
                                dedlist_ptr->nodes[ind].next);
        }
        else
        {
            //free node
            fprintf(dot_file,   "NODE_%llu[shape=\"record\", fontname=\"verdana\",\n"
                                "style=bold, style=filled,\n"
                                "color=\"" COLOR_FREE_NODE_COLOR "\", fillcolor=\"" COLOR_FREE_NODE_FILL "\",\n"
                                "label = \""
                                "{ <i>ind: %llu }|{ data:\\nfree} | {<p>prev:\\n%td |<n>next:\\n%td}"
                                "\"];\n\n",
                                ind,
                                ind,
                                dedlist_ptr->nodes[ind].prev,
                                dedlist_ptr->nodes[ind].next);
        }
    }
    //---------------------------------------------------------------------


    //-------------------------------SPECIAL_LABELS------------------------
    fprintf(dot_file,   "FICTIONAL[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n"
                        "HEAD[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n"
                        "TAIL[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n"
                        "FREE[shape=tripleoctagon, style=filled,\n"
                        "fontname=\"verdana\", color=\"" COLOR_LABEL_COLOR "\", fillcolor=\"" COLOR_LABEL_FILL "\"];\n\n\n");
    //---------------------------------------------------------------------


    //-----------------------------------NODES_SEWING----------------------
    for (size_t ind = 0; ind < dedlist_ptr->capacity - 1; ind++)
    {
        fprintf(dot_file,   "NODE_%lld->NODE_%lld[weight=10, color=\"" COLOR_BG "\"];\n",
                            ind, ind+1);
    }
    //---------------------------------------------------------------------


    //----------------------------SPECIAL_LABELS_RANKS---------------------
    fprintf(dot_file,   "{ rank=same; FICTIONAL; NODE_0; }\n"
                        "{ rank=same; HEAD; NODE_%td; }\n"
                        "{ rank=same; TAIL; NODE_%td; }\n"
                        "{ rank=same; FREE; NODE_%td; }\n\n\n",
                        dedlist_get_head_ind(dedlist_ptr),
                        dedlist_get_tail_ind(dedlist_ptr),
                        dedlist_ptr->free );
    //---------------------------------------------------------------------


    //--------------------------------------EDGES--------------------------
    for (size_t ind = 0; ind < dedlist_ptr->capacity; ind++)
    {
        // TODO - вынести в ф-ю проверку на то, какой тип ячейки
        if ( dedlist_ptr->nodes[ind].prev != -1 )
        {
            // occupied node
            fprintf(dot_file,   "NODE_%lld:<p>->NODE_%td[color=\"" COLOR_EDGE_PREV "\", penwidth=2];\n"
                                "NODE_%lld:<n>->NODE_%td[color=\"" COLOR_EDGE_NEXT "\", penwidth=2];\n\n",
                                ind,
                                dedlist_ptr->nodes[ind].prev,
                                ind,
                                dedlist_ptr->nodes[ind].next);
        }
        else
        {
            // free node
            fprintf(dot_file,   "NODE_%lld:<n>->NODE_%lld[color=\"" COLOR_EDGE_FREE "\", penwidth=2];\n",
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

inline DedlistStatusCode generate_dump_img_( )
{
    char cmd[MAX_CMD_GEN_DUMP_IMG_LENGHT] = {};
    int written_chars = snprintf(   cmd,
                                    MAX_CMD_GEN_DUMP_IMG_LENGHT,
                                    "dot %s -Tjpg -o %s",
                                    DUMP_DOT_FILE_PATH,
                                    DUMP_IMG_FILE_PATH);

    printf("<%s>\n", cmd);

    if ( written_chars >= (int) MAX_CMD_GEN_DUMP_IMG_LENGHT)
    {
        fprintf(stderr, "Command to generate dump image is too long, can't execute it. "
        "Please, make paths to files shorter.\n");
        return DEDLIST_STATUS_ERROR_TOO_LONG_CMD_GEN_DUMP_IMG;
    }

    // TODO - разобраться почему не работает!
    //system(cmd);

    system( "dot .\\dumps\\dedlist_dump.dot -Tjpg -o .\\dumps\\dedlist_dump.jpg" );
    fprintf(stderr, "Dedlist dump image is generated!\n");

    return DEDLIST_STATUS_OK;
}

inline DedlistStatusCode show_dump_img_( )
{
    system(DUMP_IMG_FILE_PATH);

    return DEDLIST_STATUS_OK;
}

inline DedlistStatusCode free_dot_file_( FILE * dot_tmp_file )
{
    fclose(dot_tmp_file);

    // TODO - удаление временного файла (или добавление подписи со временем)
    return DEDLIST_STATUS_OK;
}

void dedlist_dump_( VoidDedlist *dedlist_ptr,
                    dl_verify_res_t verify_res,
                    const char *file,
                    const int line,
                    const char *func )
{

    FILE *dot_file = NULL;

    // TODO - следить за возвращенным DedlistStatus
    create_tmp_dot_file_( DUMP_DOT_FILE_PATH, &dot_file );

    write_dot_file_for_dump_(dot_file, dedlist_ptr, verify_res, file, line, func );

    generate_dump_img_( );

    free_dot_file_(dot_file);

    show_dump_img_( );
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
