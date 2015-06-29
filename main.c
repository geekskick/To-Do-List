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

#define HASH_MAX                100
#define MAX_IN                  100

#define SUCCESS                 1
#define FAILURE                 0

#define TRUE                    1
#define FALSE                   0

#define DEFAULT_NAME            "Default"

#define G_STR_IN(a)             if(disp_input(&str_in, a) == FAILURE){ \
                                    fprintf(stderr, "[line %d] Error getting input\n", __LINE__); \
                                    return FAILURE; \
                                }

//return value is null
#define MEM_CHK(a, ...)         if (!a) { \
                                    fprintf(stderr, "[line %d] Error assigning memory for [%s] item\n", __LINE__, __VA_ARGS__); \
                                    return NULL; \
                                }

//returns an integer code
#define MEM_CHK_I(a, ...)       if (!a) { \
                                    fprintf(stderr, "[line %d] Error assigning memory for [%s] item\n", __LINE__, __VA_ARGS__); \
                                    return FAILURE; \
                                }

#define print_list(a)           print_list_recursive(a, TRUE)

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
uchar is_in_list(record **header, char* search_term);
void dest_list_recursive(record *header);
void print_list_recursive(record* header, uchar frame_needed);
uchar add_item(record** header);
uchar get_hash_input(record* map[HASH_MAX]);
unsigned int RSHash(char* str, unsigned int len);
uchar disp_input(char **str_in, const char *message);
void show_menu();
record* find_last(record **header);
void free_node(record* node);

//--------- Functions ----------


/*
 Frees up the HEAP memory occupied by the node and it's text
 */
void free_node(record* node){
    if (!node) {
        return;
    }
    
    free(node->text);
    node->text = NULL;
    
    free(node);
    node = NULL;
}


/*
 loops through the linked list to find the next null element, then 
 returns the address of it.
 
 Must be passed a node which isn't NULL otherwise errors will happen
 
 Must pass back the node before so that it can be linked forward
 */
record* find_last(record** header){
    
    if ((*header)->next) {
        return find_last(&(*header)->next);
    }
    
    return *header;
}

/*
 Create memory space for an item and return a pointer to it
 */

record* create_record(const char* text){
    
    record *new = calloc(1, sizeof(record));
    MEM_CHK(new, text);
    
    new->text = calloc(strlen(text) + 1, sizeof(char));
    MEM_CHK(new, text);
    
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
    
    /*
     get the address of the last element in the linked list,
     then link it forward to the new one
     */
    record *last = find_last(header);
    
    last->next = create_record(text);
    if (!last->next) {
        return FAILURE;
    }

    return SUCCESS;
    
}

/*
 Free the memory in the list
 */
void dest_list_recursive(record *node){
    
    /*
     If the next item is null if must be the last item in the list, so 
     free it, then pop back to the previous item and free it, etc.
     
     Since the text and the node itself are on the HEAP bot must be freed, 
     cant free the text afterwards
     */
    if (node->next) {
        dest_list_recursive(node->next);
    }
    printf("Destroying: %s\n",node->text);
    
    free_node(node);
}

/*
 Print the items in the list
 */
void print_list_recursive(record* header, uchar frame_needed){
    
    /*
     if the node passed is null then its the last in the linked list, therefore stop recursion.
     the function is only over called using the macro print_list, which passes a 1, indicating the 
     node passed was the first of the link list, and therefore the category.
     */
    if (!header) {
        printf("End of list\n");
        return;
    }
    else if (frame_needed == TRUE) {
        printf("------------\n%s\n", header->text);
    }
    else{
        printf("Item: %s\n", header->text);
    }
    
    print_list_recursive(header->next, FALSE);
}

/*
    get stdin and add the text in it to the end of the list
 */
uchar add_item(record** header){
    
    char* input_buff = calloc(MAX_IN, sizeof(char));
    MEM_CHK_I(input_buff, "Input memory");
    
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
    MEM_CHK_I(str_in, "Input memory");
    
    scanf("%s", *str_in);
    
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
                          MEM_CHK_I(in_str, "in_str");
    
    unsigned int code   = 0;

    options menu_choice = 0;
    
    
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
        fprintf(stderr, "<Invalid Input>\n");
    }
    else{
        menu_choice = int_in;
    }
    
    switch (menu_choice) {
            
        case add:
            G_STR_IN("Which category?");
            
            //create category if needs be
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in) + 1))] == NULL) {
                if(!(map[code] = create_record(str_in))){
                    return FAILURE;
                }
            }
            
            printf("Enter the new item\n> ");
            
            if ((int_in = add_item(&map[code])) == FAILURE){
                fprintf(stderr, "Error in get_input() function\n");
            }
            
            break;
            
        case delete:
            G_STR_IN("Which category?");
            
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in) + 1))] == NULL) {
                printf("Category doesn't exist\n");
                int_in = SUCCESS;
                break;
            }
            
            /*
             Clear the str_in buffer and use it for a new input
             */
            memset(str_in, 0, MAX_IN);
            printf("Enter the item\n> ");
            scanf("%s", str_in);
            
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
                if (map[i]) {
                    int_in = is_in_list(&map[i], str_in);
                    if (int_in == SUCCESS) {
                        printf("[%s] is in category [%s]\n", str_in, map[i]->text);
                    }
                }
            }
            
            if(int_in == FAILURE){
                printf("%s isn't an item\n", str_in);
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
uchar is_in_list(record **header, char* search_term){
    
    record *current = NULL;
    
    /*
     iterate through the list until the NULL is found, each time
     comparing the text to the search term, strcmp returns 0 if successful
     */
    for (current = *header; current != NULL; current = current->next){
        
        if(strcmp(current->text, search_term) == 0){
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
            
            free_node(current);
            
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
    if((map[RSHash(DEFAULT_NAME, (unsigned int)strlen(DEFAULT_NAME) + 1)] = create_record(DEFAULT_NAME)) == NULL){
        return EXIT_FAILURE;
    }
    
    //infinite loop of main program
    while (get_hash_input(map) != FAILURE) {
    //do nothing
    };
    
    printf("----- Exiting -----\n");
    
    for (int i = 0; i < HASH_MAX; i++) {
        if (map[i] != NULL) {
            dest_list_recursive(map[i]);
        }
    }
    
    printf("----- Bye! -----\n");

    return EXIT_SUCCESS;
}
