#ifndef STRING_VECTOR_H
#define STRING_VECTOR_H

#include <stddef.h> //size_t 
#include <stdlib.h>
#include <string.h>

typedef struct StringVector {
   char **arr;
   size_t length; 
   size_t capacity;
} StringVector;

//initialize sv (array NULL length 0)
//(no allocation or deallocation is performed)
void sv_init(StringVector *sv);

//deallocates and clears sv as though it was initialized (array NULL length 0)
void sv_clear(StringVector *sv);

//adds str as last cell of sv.
//assumes str is a dynamically allocated C-String.
void sv_add_last(StringVector *sv, char *str);

//remove string at index. does not shrink allocation size
//assumes index is in range
void sv_remove_at(StringVector *sv, size_t index);

// my addition: internally facilitates growing array capacity (exponentially) when required
static void sv_grow(StringVector *sv);

#endif