#include "string_vector.h"

//initialize sv (array NULL length 0)
void sv_init(StringVector *sv)
{
    sv->capacity = 0;
    sv->length = 0;
    sv->arr = NULL;
}

//deallocates and clears sv as though it was initialized (array NULL length 0)
void sv_clear(StringVector *sv)
{
    if (sv == NULL) return;

    for (int i = 0; i < sv->capacity; i++)
    {
        if (sv->arr[i] == NULL) continue;
        free(sv->arr[i]);
    }

    free(sv->arr);
    sv->length = 0;
    sv->capacity = 0;
    sv->arr = NULL;
}

//adds str as last cell of sv.
//assumes str is a dynamically allocated C-String.
void sv_add_last(StringVector *sv, char *str)
{
    if (sv->length == sv->capacity)
    {
        sv_grow(sv);
    }

    sv->arr[sv->length] = str;
    sv->length++;
}

//remove string at index. does not shrink allocation size
void sv_remove_at(StringVector *sv, size_t index)
{
    if (index < 0 || index >= sv->length || sv->arr[index] == NULL) return;

    for (int i = index; i < sv->length-1; i++)
    {
        sv->arr[i] = sv->arr[i+1];
    }

    sv->arr[sv->length-1] = NULL;
    sv->length--;
}

static void sv_grow(StringVector *sv)
{
    char** newArr;
    size_t newCapacity;

    if (sv->capacity > 0)
    {
        newCapacity = 4;
    }
    else
    {
        newCapacity = sv->capacity * 2;
    }

    newArr = malloc(sizeof(char*) * newCapacity);

    for (int i = 0; i < sv->capacity; i++)
    {
        if (i < sv->length)
        {
            newArr[i] = malloc(sizeof(char) * strlen(sv->arr[i]));
            strcpy(sv->arr[i], newArr[i]);
        }
        else
        {
            newArr[i] = NULL;
        }

        free(sv->arr[i]);
    }

    free(sv->arr);
    sv->arr = newArr;
    sv->capacity = newCapacity;
}
