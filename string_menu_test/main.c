#include "string_vector.h"
#include "string_functions.h"

//TODO: ADD a typedef to StringVectorAction here
;;

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
    MenuItem menu[] = ;; 

    //TODO: menu loop
    ;;

    //TODO: make sure to free any dynamically allocated memory
    ;;
}


//TODO: complete the action_* functions below

void action_add_string(StringVector *sv)
    ;;

void action_del_string(StringVector *sv)
    ;;

void action_rep_char(StringVector *sv)
    ;;

void action_rep_char_all(StringVector *sv)
    ;;

void action_zigzag_copy(StringVector *sv)
    ;;

