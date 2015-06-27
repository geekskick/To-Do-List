//
//  main.c
//  Todo List
//
//  http://www.reddit.com/r/dailyprogrammer/comments/39ws1x/20150615_challenge_218_easy_todo_list_part_1/
/*
 
 Uses a hashtable to keep track of categories, where each element is the start of a singley linked list
 
 Hash table:
 where the list names are the categories
  ___________    ___________     ___________
 |          |   |           |   |           |
 | Default  |-->|  List 1   |-->|   List n  |
 |__________|   |___________|   |___________|
       |              |               |
 Linked Lists:        |               |
       |              |               |
  _____V____     _____V_____     _____V_____
 |          |   |           |   |           |
 | item 1   |   |  ITEM 1   |   | ITEM 1    |
 |__________|   |___________|   |___________|
       |              |               |
  _____V____          V          _____V_____
 |          |        NULL       |           |
 | item 2   |                   |   ITEM 2  |
 |__________|                   |___________|
       |                              |
  _____V____                          V
 |          |                       NULL
 | item 3   |
 |__________|
       |
       V
     NULL


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HASH_MAX    100
#define MAX_IN      100

#define SUCCESS     1
#define FAILURE     0

#define DEFAULT_NAME "Default"

#define G_STR_IN(a)             if(disp_input(&str_in, a) == FAILURE){ \
                                    fprintf(stderr, "Error getting input [line %d]\n", __LINE__); \
                                    return EXIT_FAILURE; \
                                }

// -------- Typedefs -----------
typedef struct record record;
typedef uint8_t uchar;
typedef int8_t  schar;
typedef enum { add = 1, delete, print, find, show_cat, search, end } options;

struct record {
    record *next;
    char *text;
};

// --------- Prototypes ---------

record* create_record(const char* text);
uchar add_to_list(record **header, const char* text);
uchar delete_item(record **header, char* item_text);
uchar is_in_list(record **header, char* text);
void destroy_list(record *header);
void print_list(record* header);
uchar add_item(record** header);
uchar get_hash_input(record* map[HASH_MAX]);
unsigned int RSHash(char* str, unsigned int len);
uchar disp_input(char **str_in, const char *message);
void show_menu();

//--------- Functions ----------

/*
 Create memory space for an item and return a pointer to it
 */
record* create_record(const char* text){
    
    record *new = calloc(1, sizeof(record));
    
    if (!new) {
        fprintf(stderr, "Error assigning memory for %s item \n", text);
        return NULL;
    }
    
    new->text = calloc(strlen(text) + 1, sizeof(char));
    if (!new->text) {
        fprintf(stderr, "Error assigning memory for %s item \n", text);
        free(new);
        return NULL;
    }
    
    strcpy(new->text, text);
    return new;
    
}

/*
 Add an item to the list, always in the end position
 If it fails return FAILURE
 */
uchar add_to_list(record **header, const char* text){
    
    /*
     If it's the first item in the list the thing in header will be NULL
     this is always going to be the category label
     */
    if (!header) {
        *header = create_record(text);
        if (!*header) {
            return FAILURE;
        }
        return SUCCESS;
    }
    
    record *last = *header;
    
    /*
     iterate through until the ->next is NULL, then add a new record in it's place
     */
    for (last = *header; last->next != NULL; last = last->next) {
        //do nothing
    }
    
    last->next = create_record(text);
    if (!last->next) {
        return FAILURE;
    }

    return SUCCESS;
    
}

/*
 Free the memory in the list
 */
void destroy_list(record *header){
    
    record  *last = header,
            *temp = NULL;

    /*
      free the memory for both the record and it's text until the NULL is encountered,
     setting each pointer to NULL after free() to avoid confusion later on.
     
     print the item being destroyed to keep the user aware of the items they had to do
     Or for debugging
     */
    
    for (last = header; last != NULL; last = temp) {
        
        printf("Destroying item: %s\n",last->text);
        
        temp = last->next;
        
        free(last->text);
        last->text = NULL;
        
        free(last);

    }
}

/*
    Print the items in the list
 */
void print_list(record* header){
    
    record *last = header;
    int i = 0;
    
    /*
     the first element of the list (header) is the name of the category, 
     all other are items within it, frame the list for clarity
     */
    printf("------------\n");
    if (!last) {
        printf("No Items\n");
    }
    
    for (; last != NULL; last = last->next, i++) {
        if (i == 0) {
            printf("Category %s\n", last->text);
            i++;
        }
        else{
            printf("Item %d: %s\n", i, last->text);
        }
    }

    printf("------------\n");
}

/*
    get stdin and add the text in it to the end of the list
 */
uchar add_item(record** header){
    
    char* input_buff = calloc(MAX_IN, sizeof(char));
    if (!input_buff) {
        fprintf(stderr, "Error getting input memory\n");
        return FAILURE;
    }
    
    scanf("%s",input_buff);
    
    int rc = add_to_list(header, input_buff);
    
    free(input_buff);
    
    return rc;
    
}

/*
    disaplys a message on the screen then gets the input and
    puts it in str_in, retruning appropraite code
 */
uchar disp_input(char **str_in, const char *message){
    
    printf("%s\n>", message);
    *str_in = calloc(MAX_IN, sizeof(char));
    
    if (!*str_in) {
        fprintf(stderr, "Error getting memory for string\n");
        return FAILURE;
    }
    
    scanf("%s",*str_in);
    
    return SUCCESS;
}

/* display menu options on the screen
 
 1 - Add item
 2 - Delete Item
 3 - Print Category
 4 - Find in category
 5 - show categories
 6 - search all categories
 7 - exit
 
 */
void show_menu(){
    
    printf("1 - Add item\n");
    printf("2 - Delete item\n");
    printf("3 - Print Category\n");
    printf("4 - Find in category\n");
    printf("5 - Show Categories\n");
    printf("6 - Search in all\n");
    printf("7 - Exit\n> ");
}

/*
 main menu function
 returns EXIT_FAILURE if a fatal error occurred
 */
uchar get_hash_input(record* map[HASH_MAX]){
    
    int
         int_in         = 0,
         n_options      = 0;
    char
         *str_in        = NULL,
         *in_str        = calloc(MAX_IN, sizeof(char));
    
    unsigned int code   = 0;
    record* curr        = NULL;
    options menu_choice = 0;
    
    if (!in_str) {
        fprintf(stderr, "Error getting in_str memory");
        return FAILURE;
    }
    
    /*
     iterate though the menu choices to find the maximum, 
     used for validating user input
     */
    for (int i = 0; n_options < end; ++i) {
        n_options = i;
    }
    
    /*
     in order to ensure valid user input it is read in as a string, 
     then converted to an int using strtol(), reading it in as an int 
     gives the option of a never ending loop if the user enters 
     a character, for example
     */
    
    show_menu();
    scanf("%s",in_str);
    fflush(stdin);
    
    int_in = (int)strtol(in_str, NULL, 10);
    
    /*
     test user input, if it is in range of the enum then that's cool, 
     if not display inavlid and leave the menu_choice at 0, 
     this will default in the switch case
     */
    if (int_in < 1 || int_in > n_options) {
        fprintf(stderr, "Invalid Input\n");
    }
    else{
        menu_choice = int_in;
    }
    
    switch (menu_choice) {
            
        case add:
            G_STR_IN("Which category?");
            
            //create category if needs be
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in) + 1))] == NULL) {
                map[code] = create_record(str_in);
            }
            
            printf("Enter the new item\n> ");
            
            if ((int_in = add_item(&map[code])) == FAILURE){
                fprintf(stderr, "Error in get_input() function\n");
            }
            
            break;
            
        case delete:
            G_STR_IN("Which category?");
            
            scanf("%s",str_in);
            
            //create category if needs be
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in) + 1))] == NULL) {
                printf("Category doesn't exist\n");
                int_in = SUCCESS;
                break;
            }
            
            if((int_in = delete_item(&map[code], str_in)) == FAILURE){
                fprintf(stderr, "No match found for %s\n",str_in);
                int_in = SUCCESS;
                
            }
            break;
            
        case print:
            G_STR_IN("Which category?");
            
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in) + 1))] == NULL){
                printf("%s isn't a category\n",str_in);
                int_in = SUCCESS;
                break;
            }
            
            print_list(map[code]);
            break;
            
        case find:
            G_STR_IN("Which category?");
            
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in) + 1))] == NULL){
                printf("%s isn't a category\n",str_in);
                int_in = SUCCESS;
                break;
            }
            
            /*
             Clear the str_in buffer and use it for a new input
             */
            memset(str_in, 0, MAX_IN);
            printf("What item is it in?\n> ");
            scanf("%s",str_in);
            
            
            if ((int_in = is_in_list(&map[code], str_in)) == FAILURE) {
                printf("%s is not in the list\n",str_in);
                int_in = SUCCESS;
            }
            else{
                printf("%s is in the list\n", str_in);
            }
            
            break;
           
        case show_cat:
            printf("-----------\n");
            for (int i = 0; i < HASH_MAX; ++i) {
                if (map[i] != NULL) {
                    printf("> %s\n",map[i]->text);
                }
            }
            printf("-----------\n");
            break;
        
        case end:
            
            int_in = FAILURE;
            break;

        case search:
            G_STR_IN("What is it?");
            
            printf("Searching (this may take a while)\n");
            
            /*
             Set input to FAILURE, when a match is found change it back to EXIT_SUCCESS to act as a
             'match found' flag for the search.
             
             iterate though the hashmap and explore the only the lists which aren't NULL
             
             */
            int_in = FAILURE;
            
            for (int i = 0; i< HASH_MAX && int_in == FAILURE; ++i) {
                
                //list must have content
                if (map[i]) {
                    
                    //first item is category name
                    curr = map[i]->next;
                    
                    //loop through until a NULL is found showing the end of the list
                    while (curr!= NULL) {
                        
                        //is the text a match?
                        if (strcmp(curr->text, str_in) == 0) {
                            printf("Found %s in %s\n",curr->text, map[i]->text);
                            int_in = SUCCESS;
                            break;
                        }
                        
                        //point to next item in list
                        curr = curr->next;
                    }
                }
            }
            
            if(int_in == FAILURE){
                printf("%s isn't an item\n",str_in);
                int_in = SUCCESS;
            }
            break;
            
            //incase of erroneuous menu choice by user
        default:
            int_in = SUCCESS;
            break;
    }
    
    free(in_str);
    in_str = NULL;
    
    if (str_in) {
        free(str_in);
        str_in = NULL;
    }
    
    return int_in;
}

/*
 returns EXIT_SUCCESS if the text  passed is an item in the list
 */

uint8_t is_in_list(record **header, char* text){
    record *current = NULL;
    
    for (current = *header; current != NULL; current = current->next){
        if(strcmp(current->text, text) == 0){
            return SUCCESS;
        }
    }
    return FAILURE;
}


/*
 deletes an item from the list
 returns EXIT_SUCCESS if it worked, if the item not found returns EXIT_FAILURE
 */
uint8_t delete_item(record **header, char* item_text){
    
    record *current = NULL, *prev = NULL;
    
    /*
     iterate through the list, keeping note of the previous item and the current item,
     stop when the end is reached.
     
     if the first element is the thing to be deleted the 'prev' will still be NULL.
     
     point the previous item to the one after the item to be matched
     
     Previous           Match           Next
        v                                ^
        |________________________________|
     
     Then free the match
     
     */
    for (current = *header; current != NULL; prev = current, current = current->next) {
        if (strcmp(current->text, item_text) == 0) {
            
            if (prev == NULL) { //must be first element in the list
                *header = current->next;
            }
            else {
                prev->next = current->next;
            }
            
            free(current);
            return SUCCESS;
        }
    }
    
    return FAILURE;
}

/* 
 algorithm from wikipedia to generate the hash code in range of HASH_MAX
 */
unsigned int RSHash(char* str, unsigned int len){
    
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    unsigned int hash = 0;
    unsigned int i    = 0;
    
    for(i = 0; i < len; str++, i++)
    {
        hash = hash * a + (*str);
        a    = a * b;
    }
    
    return hash % HASH_MAX;
}

int main(int argc, const char * argv[]) {
    
    printf("Welcome to the To-Do list manager\n");
    
    //initialise the hash table to null elements
    record *map[HASH_MAX];
    for (int i = 0; HASH_MAX > i; ++i) {
        map[i] = NULL;
    }
    
    //create default category
    map[RSHash(DEFAULT_NAME, (unsigned int)strlen(DEFAULT_NAME) + 1)] = create_record(DEFAULT_NAME);

    //infinite loop of main program
    while (get_hash_input(map) != FAILURE) {
    //do nothing
    };
    
    printf("----- Exiting -----\n");
    
    for (int i = 0; i < HASH_MAX; ++i) {
        if (map[i] != NULL) {
            destroy_list(map[i]);
        }
    }

    return 0;
}
