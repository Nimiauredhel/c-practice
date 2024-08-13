#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "string_vector.h"
#include "string_functions.h"

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
    // Initializing the string vector
    StringVector* sv = malloc(sizeof(StringVector));
    sv_init(sv);

    // Initializing the menu array
    const unsigned char numItems = 5;
    const MenuItem menu[] = 
    {
        {
            .description = "Add string",
            .action = action_add_string
        },
        {
            .description = "Delete string",
            .action = action_del_string
        },
        {
            .description = "Replace character",
            .action = action_rep_char
        },
        {
            .description = "Replace character from all",
            .action = action_rep_char_all
        },
        {
            .description = "Zigzag copy",
            .action = action_zigzag_copy
        }
    };

    while(1)
    {
        // begin menu loop
        unsigned int actionIndex;
        unsigned int i;
        printf("\n----------\n");

        // Print the strings in the vector
        for (i = 0; i < sv->length; i++)
        {
            printf("%u) %s", i, sv->arr[i]);
        }

        printf("\n");

        // Print the menu options
        for (i = 0; i < numItems; i++)
        {
            printf("%u) %s\n", i+1, menu[i].description);
        }

        printf("%u) Quit\nSelect option: ", 0);

        // Debug stuff
        printf("\nLength: %lu, Capacity: %lu\n", sv->length, sv->capacity);

        // Read input
        scanf(" %u", &actionIndex);

        if (actionIndex == 0)
        {
            break;
        }
        else if (actionIndex <= numItems)
        {
            menu[actionIndex-1].action(sv);
        }
    }

    // Making sure to free any dynamically allocated memory
    // (for the exercise, since it serves no purpose at this point)
    sv_clear(sv);
    free(sv);

    return 0;
}

void action_add_string(StringVector *sv)
{
    char *inputStr = NULL;
    size_t buffSize = 0;
    printf("\nPlease provide the new string to add.\n");
    // calling the function twice to consume the remaining newline
    getline(&inputStr, &buffSize, stdin);
    getline(&inputStr, &buffSize, stdin);
    sv_add_last(sv, inputStr);
}

void action_del_string(StringVector *sv)
{
    unsigned int idx;
    printf("\nPlease provide the index of the string to remove.\n");
    scanf(" %u", &idx);
    sv_remove_at(sv, idx);
}

void action_rep_char(StringVector *sv)
{
    unsigned int idx;
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
        printf("\n\nRequested index is outside array bounds.\n\n");
        return;
    }

    printf("\nPlease provide the character that you would like to be replaced.\n");
    scanf(" %c", &src);
    printf("\nPlease provide the character that you would like to replace the previous character with.\n");
    scanf(" %c", &dst);

    char* str = sv->arr[idx];
    size_t length = strlen(str);

    for (unsigned int i = 0; i < length; i++)
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

    printf("\nPlease provide the character that you would like to be replaced.\n");
    scanf(" %c", &src);
    printf("\nPlease provide the character that you would like to replace the previous character with.\n");
    scanf(" %c", &dst);

    for (unsigned int i = 0; i < sv->length; i++)
    {
        char* str = sv->arr[i];
        size_t length = strlen(str);

        for (unsigned int j = 0; j < length; j++)
        {
            if (str[j] == src) str[j] = dst;
        }
    }
}

void action_zigzag_copy(StringVector *sv)
{
    unsigned int first;
    unsigned int second;

    if (sv->length <= 0)
    {
        printf("\n\nArray is empty.\n\n");
        return;
    }

    printf("\nPlease select the first string index.\n");
    scanf(" %u", &first);
    printf("\nPlease select the second string index.\n");
    scanf(" %u", &second);

    if (sv->length <= first || sv->length <= second)
    {
        printf("\n\nArray is empty.\n\n");
        return;
    }

    char* strFinal;

    {
        char* strFirst = sv->arr[first];
        char* strSecond = sv->arr[second];
        size_t lengthFirst = strlen(strFirst);
        size_t lengthSecond = strlen(strSecond);
        size_t lengthFinal = lengthFirst+lengthSecond;
        strFinal = malloc(sizeof(char)*(lengthFinal+1));

        size_t idxOrigin = 0;

        for(unsigned int i = 0; i < lengthFinal && idxOrigin < lengthFirst;
                i < lengthSecond ? i+=2 : i++, idxOrigin++)
        {
            strFinal[i] = strFirst[idxOrigin];
        }

        idxOrigin = 0;

        for(unsigned int i = 0; i < lengthFinal && idxOrigin < lengthSecond;
                i < lengthFirst ? i+=2 : i++, idxOrigin++)
        {
            strFinal[i+1] = strSecond[idxOrigin];
        }

        strFinal[lengthFinal] = '\0';

    }

    sv_add_last(sv, strFinal);
}

