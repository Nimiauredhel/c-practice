#include <stdio.h>
#include <stdint.h>
#include "string_vector.h"
#include "string_functions.h"

//TODO: ADD a typedef to StringVectorAction here
typedef void (*StringVectorAction)(StringVector *sv);

typedef struct MenuItem {
    const char* description;
    StringVectorAction action;
} MenuItem;

//String Vector Actions
//
//reads line from user and adds as a new string at end of vector
void action_add_string(StringVector *sv);
//reads index from user and deletes that string from the vector
void action_del_string(StringVector *sv);
//reads from input: index, and two characters
//inside string at index replaces all instances of the first character
//with the second character
void action_rep_char(StringVector *sv);
//reads from input: two characters
//replaces all instances of the first character
//found in any string in sv with the second character.
void action_rep_char_all(StringVector *sv);
//reads from input: two indexes
//adds a new string to sv that is a zigzag copy
//of the two string in the indexes
//(see helper.h:str_new_zigzag)
void action_zigzag_copy(StringVector *sv);

int main(void) { 
    //TODO: initialize a string vector
    ;;

    //TODO: initialize the menu array
    MenuItem menu[] = 
    {
        {
            .description = "Add String",
            .action = action_add_string
        }
    };

    //TODO: menu loop
    ;;

    //TODO: make sure to free any dynamically allocated memory
    ;;
}


//TODO: complete the action_* functions below

void action_add_string(StringVector *sv)
{
    char *str;
    printf("Please provide the new string to add.\n");
    scanf(" %s", str);
    sv_add_last(sv, str);
}

void action_del_string(StringVector *sv)
{
    uint16_t idx;
    printf("Please provide the index of the string to remove.\n");
    scanf(" %u", &idx);
    sv_remove_at(sv, idx);
}

void action_rep_char(StringVector *sv)
{
    uint idx;
    char src;
    char dst;

    if (sv->length <= 0)
    {
        printf("\n\nArray is empty.\n\n");
        return;
    }

    printf("\nPlease provide the index of the string to alter.\n");
    scanf(" %u", &idx);
    
    if (sv->length <= idx)
    {
        printf("\n\nRequested index is outside array bounds.\n\n";
        return;
    }

    printf("\nPlease provide the character that you would like to be replaced.\n");
    scanf(" %c", src);
    printf("\nPlease provide the character that you would like to replace the previous character with.\n");
    scanf(" %c", dst);

    char* str = sv->arr[idx];
    size_t length = strlen(str);

    for (int i = 0; i < length; i++)
    {
        if (str[i] == src) str[i] = dst;
    }
}

void action_rep_char_all(StringVector *sv)
{
    char src;
    char dst;

    if (sv->length <= 0)
    {
        printf("\n\nArray is empty.\n\n");
        return;
    }

    for (int i = 0; i < sv->length; i++)
    {
        char* str = sv->arr[i];
        size_t length = strlen(str);

        for (int j = 0; j < length; j++)
        {
            if (str[j] == src) str[j] = dst;
        }
    }
}

void action_zigzag_copy(StringVector *sv)
{
    uint first;
    uint second;

    if (sv->length <= 0)
    {
        printf("\n\nArray is empty.\n\n");
        return;
    }

    if (sv->length <= first || sv->length <= second)
    {
        printf("\n\nArray is empty.\n\n");
        return;
    }

    {
        char* strFirst = sv->arr[first];
        char* strSecond = sv->arr[second];
        size_t lengthFirst = strlen(strFirst);
        size_t lengthSecond = strlen(strSecond);
        size_t lengthFinal = lengthFirst+lengthSecond;
    }
}

