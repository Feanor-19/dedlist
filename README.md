# dedlist
Educational project at MIPT. Non-classic implementation of double-linked list. 

## Brief description
This double-linked list allows you to insert, delete and access any elements (not only in head and tail) for O(1). It is achieved in the following way: when you insert something in the list, 
you get its `anchor`. You can use this `anchor` to access the element, to delete this element or to insert a new element.

## Usage 
Change the following lines in `dedlist.h`:
- `typedef int Elem_t;`
- `const Elem_t ELEM_T_DEFAULT_VALUE = 0;`
- `inline void dedlist_print_elem_t(FILE *stream, Elem_t value)
{
    fprintf(stream, "%d", value);
}`
so that they describe the data type you want to store in the list.

You can also define following macros to change some behaviour:
- `DEDLIST_DO_DUMP` Turns on dumps and vericiations.
- `DEDLIST_SHOW_DUMP_IMG` Turns on immediate showing of dump image as it is generated. Requires `DEDLIST_DO_DUMP`.

## Note concerning dumps
`graphviz dot` is used to generate dump images! So, if you define macro `DEDLIST_DO_DUMP`, you should have `dot` installed and specified in PATH. Here is an example of an image dump:
![dmp](https://github.com/Feanor-19/dedlist/assets/142310227/399c7c0b-9a29-428c-9ca2-5c7e610a972f)
