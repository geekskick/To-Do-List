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
 | Default  |-> |  List 1   |-> |   List n  |
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

#define HASH_MAX 100
#define MAX_IN 100

// -------- Typedefs -----------
typedef struct record record;

struct record {
    record *next;
    char *text;
};

// --------- Prototypes ---------

record* create_record(const char* text);
uint8_t add_to_list(record **header, const char* text);
uint8_t delete_item(record **header, char* item_text);
uint8_t is_in_list(record **header, char* text);
void destroy_list(record *header);
void print_list(record* header);
uint8_t add_item(record** header);
uint8_t get_input(record** header);
unsigned int RSHash(char* str, unsigned int len);
uint8_t disp_input(char **str_in, const char *message);

//--------- Functions ----------

/*
 Create memory space for an item and return a pointer to it
 */
record* create_record(const char* text){
    
    record *new = calloc(1, sizeof(record));
    new->text   = calloc(strlen(text), sizeof(char));
    
    if (!new->text || !new) {
        fprintf(stderr, "Error assigning memory for %s item \n", text);
        return NULL;
    }
    
    strcpy(new->text, text);
    return new;
    
}

/*
 Add an item to the list, always in the end position
 If it fails return EXIT_FAILURE
 */
uint8_t add_to_list(record **header, const char* text){
    
    /*
     If it's the first item in the list the thing in header will be NULL
     this is always going to be the category label
     */
    if (*header == NULL) {
        *header = create_record(text);
        if (!*header) {
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    
    record *last = *header;
    
    /*
     iterate through until the ->next is NULL, then add a new record in it's place
     */
    while (last->next != NULL) {
        last = last->next;
    }
    
    last->next = create_record(text);
    if (!last->next) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
    
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
    
    while(last != NULL){
        printf("Destroying item: %s\n",last->text);
        
        temp = last->next;
        
        free(last->text);
        last->text = NULL;
        
        free(last);
        last = temp;
    }
}

/*
    Print the items in the list
 */
void print_list(record* header){
    
    record *last = header;
    uint8_t i = 0;
    
    /*
     the first element of the list (header) is the name of the category, 
     all other are items within it, frame the list for clarity
     */
    printf("------------\n");
    if (!last) {
        printf("No Items\n");
    }
    while (last != NULL) {
        if (i == 0) {
            printf("Category %s\n", last->text);
            i++;
        }
        else{
            printf("Item %d: %s\n", i++, last->text);
        }
        last = last->next;
    }
    printf("------------\n");
}

/*
    get stdin and add the text in it to the end of the list
 */
uint8_t add_item(record** header){
    
    char* input_buff = calloc(MAX_IN, sizeof(char));
    if (!input_buff) {
        fprintf(stderr, "Error getting input memory\n");
        return EXIT_FAILURE;
    }
    
    scanf("%s",input_buff);
    
    int rc = add_to_list(header, input_buff);
    
    free(input_buff);
    
    return rc;
    
}

/*
 main menu function
 returns EXIT_FAILURE if a fatal error occurred
 */

uint8_t disp_input(char **str_in, const char *message){
    printf("%s\n>", message);
    *str_in = calloc(MAX_IN, sizeof(char));
    
    if (!*str_in) {
        fprintf(stderr, "Error getting memory for string\n");
        return EXIT_FAILURE;
    }
    
    scanf("%s",*str_in);
    
    return EXIT_SUCCESS;
}

uint8_t get_hash_input(record* map[HASH_MAX]){
    
    int input = 0;
    char *str_in = NULL;
    unsigned int code = 0;
    
    record* curr = NULL;
    
    /* display menu options on the screen
     
     1 - Add item
     2 - Delete Item
     3 - Print Category
     4 - Find in category
     5 - show categories
     6 - search all categories
     7 - exit
     
     */
    printf("1 - Add item\n2 - Delete item\n3 - Print Category\n4 - Find in category\n5 - Show Categories\n6 - Search in all\n7 - Exit\n> ");
    scanf("%d",&input);
    fflush(stdin);
    
    switch (input) {
            
        //Add item
        case 1:
            if(disp_input(&str_in, "Which category?") == EXIT_FAILURE){
                fprintf(stderr, "Error getting input [line %d]\n",__LINE__);
                return EXIT_FAILURE;
            };
            
            //create category if needs be
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in)))] == NULL) {
                map[code] = create_record(str_in);
            }
            
            
            printf("Enter the new item\n> ");
            
            if ((input = add_item(&map[code])) == EXIT_FAILURE){
                fprintf(stderr, "Error in get_input() function\n");
            }
            
            break;
            
        //Delete item
        case 2:
            if(disp_input(&str_in, "Which category?") == EXIT_FAILURE){
                fprintf(stderr, "Error getting input [line %d]\n",__LINE__);
                return EXIT_FAILURE;
            };
            
            scanf("%s",str_in);
            
            code = RSHash(str_in,(unsigned int) strlen(str_in));
            
            //create category if needs be
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in)))] == NULL) {
                printf("Category doesn't exist\n");
                input = EXIT_SUCCESS;
                break;
            }
            
            if((input = delete_item(&map[code], str_in)) == EXIT_FAILURE){
                fprintf(stderr, "No match found for %s\n",str_in);
                input = EXIT_SUCCESS;
                
            }
            break;
            
        //print category
        case 3:
            if(disp_input(&str_in, "Which category?") == EXIT_FAILURE){
                fprintf(stderr, "Error getting input [line %d]\n",__LINE__);
                return EXIT_FAILURE;
            };
            
            
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in)))] == NULL){
                printf("%s isn't a category\n",str_in);
                input = EXIT_SUCCESS;
                break;
            }
            
            
            print_list(map[code]);
            break;
            
            //find in category
        case 4:
            if(disp_input(&str_in, "Which category?") == EXIT_FAILURE){
                fprintf(stderr, "Error getting input [line %d]\n",__LINE__);
                return EXIT_FAILURE;
            };
            
            if (map[(code = RSHash(str_in,(unsigned int) strlen(str_in)))] == NULL){
                printf("%s isn't a category\n",str_in);
                input = EXIT_SUCCESS;
                break;
            }
            
            /*
             Clear the str_in buffer and use it for a new input
             */
            memset(str_in, 0, MAX_IN);
            printf("What item is it in?\n> ");
            scanf("%s",str_in);
            
            
            if ((input = is_in_list(&map[code], str_in)) == EXIT_FAILURE) {
                printf("%s is not in the list\n",str_in);
                input = EXIT_SUCCESS;
            }
            else{
                printf("%s is in the list\n", str_in);
            }
            
            break;
           
        //print categories
        case 5:
            printf("-----------\n");
            for (int i = 0; i < HASH_MAX; ++i) {
                if (map[i] != NULL) {
                    printf("> %s\n",map[i]->text);
                }
            }
            printf("-----------\n");
            break;
        
        //exit
        case 7:
            
            input = EXIT_FAILURE;
            break;
          
        //find in all
        case 6:
            if(disp_input(&str_in, "What is it?") == EXIT_FAILURE){
                fprintf(stderr, "Error getting input [line %d]\n",__LINE__);
                return EXIT_FAILURE;
            };
            
            printf("Searching (this may take a while)\n");
            
            /*
             Set input to EXIT_FAILURE, when a match is found change it back to EXIT_SUCCESS to act as a 
             'match found' flag for the search.
             
             iterate though the hashmap and explore the only the lists which aren't NULL
             
             */
            input = EXIT_FAILURE;
            
            for (int i = 0; i< HASH_MAX && input == EXIT_FAILURE; ++i) {
                
                //list must have content
                if (map[i]) {
                    
                    //first item is category name
                    curr = map[i]->next;
                    
                    //loop through until a NULL is found showing the end of the list
                    while (curr!= NULL) {
                        
                        //is the text a match?
                        if (strcmp(curr->text, str_in) == 0) {
                            printf("Found %s in %s\n",curr->text, map[i]->text);
                            input = EXIT_SUCCESS;
                            break;
                        }
                        
                        //point to next item in list
                        curr = curr->next;
                    }
                }
            }
            
            if(input == EXIT_FAILURE){
                printf("%s isn't an item\n",str_in);
                input = EXIT_SUCCESS;
            }
            break;
            
        default:
            printf("Error - incorrect choice\n");
            break;
    }
    
    if (str_in) {
        free(str_in);
        str_in = NULL;
    }
    
    return input;
}

/*
 returns EXIT_SUCCESS if the text  passed is an item in the list
 */

uint8_t is_in_list(record **header, char* text){
    record *current = NULL;
    
    for (current = *header; current != NULL; current = current->next){
        if(strcmp(current->text, text) == 0){
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
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
            return EXIT_SUCCESS;
        }
    }
    
    return EXIT_FAILURE;
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
    record *map[HASH_MAX] = { 0 };
    for (int i = 0; HASH_MAX > i; ++i) {
        map[i] = NULL;
    }
    
    //create default category
    map[RSHash("Default", (unsigned int)strlen("Default"))] = create_record("Default");

    //infinite loop of main program
    while (get_hash_input(map) != EXIT_FAILURE) {};
    
    printf("----- Exiting -----\n");
    
    for (int i = 0; i <HASH_MAX; ++i) {
        if (map[i] != NULL) {
            destroy_list(map[i]);
        }
    }

    return 0;
}
