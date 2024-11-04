#include "mtll.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_WORDS 100  // Maximum number of words in a line
#define MAX_WORD_LENGTH 50  // Maximum length of each word

// Global variables for mtll_list operations
struct mtll ** mtll_list = NULL;
struct mtll ** removing_list = NULL;
int mtll_size = 0;
int removed_entries = 0;
int valid_command = 0;

// Prints out mtll data based on type, pretty self-explanatory
void print_data(struct mtll * x) {
    switch (x->type) {
        case INT:
            printf("%d", x->entry.num);
            break;
        case FLT:
            printf("%.2f", x->entry.flt);
            break;
        case CHAR:
            printf("%c", x->entry.letter);
            break;
        case STR:
            printf("%s", x->entry.sentence);
            break;
        case LSPTR:
            struct mtll * ptr = x->entry.mtllptr;
            printf("{");
            while (ptr->next != NULL) {
                print_data(ptr);
                printf(" -> ");
                ptr = ptr->next;
            }
            print_data(ptr);
            printf("}");
            break;
        case NONE:
            break;
    }
}

// Finds the index of a mtll element
int index_of(struct mtll * ptr) {
    for (int i = 0; i < mtll_size; i++) {
        if (ptr->entry.mtllptr == mtll_list[i]) {
            return i;
        }
    }
    return -1;
}

// Returns an array of indices where the list at index is used in the list
int * used_elsewhere(int index) {

    // Allocate memory for the array
    int* result = (int*)malloc((mtll_size + 1) * sizeof(int));
    if (result == NULL) {
        return NULL;
    }
    
    // Result array index
    int result_index = 0; 

    // For all the lists in mtll_list...
    struct mtll* ptr = mtll_list[index];
    struct mtll* temp;
    for (int i = 0; i < mtll_size; i++) {
        // Skip the list at the given index
        if (i == index) {
            continue;
        }

        // Check if the current list contains the pointer
        temp = mtll_list[i];
        while (temp != NULL) {
            if (temp->type == LSPTR && temp->entry.mtllptr == ptr) {

                // Store the index where the pointer is found
                result[result_index++] = i;
                break;
            }
            temp = temp->next;
        }
    }

    // Add a sentinel value to the array
    result[result_index] = -1;

    // If no references to ptr were found, free the result array and return NULL
    if (result_index == 0) {
        free(result);
        return NULL;
    }

    return result;
}

// Prints the type of an mtll element
void print_type(struct mtll * x) {
    switch (x->type) {
        case INT:
            printf("int");
            break;
        case FLT:
            printf("float");
            break;
        case CHAR:
            printf("char");
            break;
        case STR:
            printf("string");
            break;
        case LSPTR:
            printf("reference");
            break;
        case NONE:
            printf("none");
            break;
    }
}

// Checks if an mtll element was removed
int is_in_removed(struct mtll * ptr) {
    int is_removed = 0;
    for (int i = 0; i < removed_entries; i ++ ) {
        if (removing_list[i] == ptr) {
            is_removed = 1;
        }
    }
    return is_removed;
}

// Returns 1 if a string can be cast to an int, 0 if otherwise
int is_intable(char * word) {

    // This is the end of the integer
    char *endptr;

    long int_value = strtol(word, &endptr, 10);

    // If the end of the integer is either the word itself 
    //or a null pointer, integer was not parsed properly
    if (*endptr != '\0' || endptr == word) {
        return 0;
    }

    // If it was parsed properly, check if it fits as an integer
    if (int_value <= 2147483647 && int_value >= -2147483648) {
        return 1;
    } else {
        return 0;
    }
}

// Printing format for an adjusted or new mtll
void new_view(struct mtll * ptr, int index) {
    if (ptr == NULL) {
        return;
    } 
    int lsptrpass = 0;
    struct mtll * ptrcopy = ptr;
    while (ptrcopy != NULL) {
        if (ptrcopy->type == LSPTR) {
            lsptrpass = 1;
        } 
        ptrcopy = ptrcopy->next;
    }

    if (lsptrpass) {
        printf("Nested %d: ", index);
    } else {
        printf("List %d: ", index);
    }

    if (ptr -> next == NULL) {
        print_data(ptr);
        printf("\n");
    } else {
        while (ptr->next != NULL) {
            if (ptr->type != LSPTR) {
                print_data(ptr);
            } else {
                printf("{List %d}", index_of(ptr));
            }
            printf(" -> ");
            ptr = ptr->next;
        }
        if (ptr->type != LSPTR) {
            print_data(ptr);
        } else {
            printf("{List %d}", index_of(ptr));
        }
        printf("\n");
    }
}

// Splits an input string from stdin into words
char **split_line_into_words() {
    valid_command = 1;
    // Allocate memory for the array of words
    char **words = (char **)malloc(MAX_WORDS * sizeof(char *));
    if (words == NULL) {
        return NULL;
    }

    // Initialize all elements of words to NULL
    for (int i = 0; i < MAX_WORDS; i++) {
        words[i] = NULL;
    }

    // Initialize the line buffer with null characters
    char line[MAX_WORD_LENGTH * MAX_WORDS];
    memset(line, '\0', sizeof(line));

    // Updates the line with the information from stdin
    fgets(line, sizeof(line), stdin);

    // Count the number of spaces in the line
    int space_count = 0;
    char * line2 = strdup(line);

    // Splits the line in terms of spaces, tabs, and newlines
    char *token = strtok(line, " \t\n");
    int word_count = 0;

    // While there are tokens, add them to the words array
    while (token != NULL && word_count < MAX_WORDS) {
        words[word_count] = strdup(token);
        if (words[word_count] == NULL) {
            // Free allocated memory before returning NULL
            for (int j = 0; j < word_count; j++) {
                free(words[j]);
            }
            free(words);
            return NULL;
        }
        word_count++;
        token = strtok(NULL, " \t\n");
    }

    // Add a null pointer to terminate the words and return the words
    words[word_count] = NULL;
    int insert_detected = (words[0] != NULL && strcmp(words[0], "INSERT") == 0);

    for (int i = 0; line2[i] != '\0'; i++) {
        if (line2[i] == '}') {
            valid_command = 0;
        } 

        // If we encounter an open curly bracket...
        if (line2[i] == '{') {
            int checkvar = 0;
            int intcheck = 0;

            // Check the rest of the input for if we encounter a }, an integer, a space or newline
            //if a space or newline are encountered before a }, it's invalid. If no integers are encountered
            //it's also invalid.
            while (checkvar == 0) {
                i += 1;
                if (line2[i] == '}') {
                    if (intcheck) {
                        checkvar = 1;
                    } else {
                        checkvar = 2;
                    }
                } else if (isdigit(line2[i])) {
                    intcheck = 1;
                } else if ((line2[i] == ' ') || (line2[i] == '\n')) {
                    checkvar = 3;
                }
            }

            if (checkvar == 1) {
                continue;
            } else {
                valid_command = 0;
            }

            continue;
        }

        if (line2[i] == ' ') {
            space_count++;

            if (space_count > 1) {
                // If more than one space is found between words
                if (insert_detected && space_count > 1) {
                    // When Inserting, add an element for a space in the next word over
                    //this logic ensures that another space is added for every space after the end
                    if (words[word_count + 1] != NULL) {
                        // Shift elements to the right starting from the last element
                        for (int j = word_count + 1; j >= 3; j--) {
                            words[j] = words[j - 1];
                        }
                    }

                    // Insert an empty string at the third index
                    words[word_count] = strdup("");
                    // Increment word count
                    word_count++;
                } else {
                    if (insert_detected) {
                        space_count += 1;
                        continue;
                    }
                    // Invalid input: more than one space between words
                    valid_command = 0;
                    break;
                }
            }
        } else {
            // Reset space count for consecutive non-space characters
            space_count = 0;
        }
    }

    if (line2[0] == ' ') {
        valid_command = 0;
    }
    free(line2);
    
    return words;
}

// Adds a list element to mtll_list if all parameters are met
void add_mtll(char ** words) {
    
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if (words[1] == NULL) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    char * potential_size = words[1];
    if (!is_intable(potential_size)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int size = atoi(potential_size);

        // Return INVALID COMMAND if the size is negative
        if (size < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else {

            // Allocate memory for the mtll list
            struct mtll *head;
            if (size != 0) {
                head = mtll_create(size);
                if (head == NULL) {
                    printf("Memory allocation failed.\n");
                    mtll_free(head);
                    return;
                }
            } 
            
            // If the size is 0 just define the mtll list
            //as a none type
            else {
                head = mtll_create(1);
                if (head == NULL) {
                    printf("Memory allocation failed.\n");
                    mtll_free(head);
                    return;
                }
                head->type = NONE;
            }

            // If the list has not been initiated yet, allocate
            //memory for the first element of the list
            if (mtll_list == NULL) {
                mtll_list = (struct mtll **)malloc(sizeof(struct mtll *) * 1);
                mtll_list[0] = NULL;
                if (mtll_list == NULL) {
                    printf("Memory allocation failed.\n");
                    free(mtll_list);
                    return;
                }
            } else {
                mtll_list = (struct mtll **)realloc(mtll_list, sizeof(struct mtll *) * (mtll_size + 1));
                mtll_list[mtll_size] = NULL;
                if (mtll_list == NULL) {
                    printf("Memory allocation failed.\n");
                    free(mtll_list);
                    return;
                }
            }

            mtll_list[mtll_size] = head;

            int x = mtll_add(head, size, mtll_list, mtll_size);
            if (x == 1) {
                new_view(head, mtll_size);
                mtll_size += 1;
            } else if (x == 0) {
                printf("INVALID COMMAND: %s\n", words[0]);
                mtll_free(head);
            } else if (x == -1) {
                mtll_free(head);
            }
        }
    }
}

// Prints out which mtlls have not been removed and have been instantiated
void view_all() {
    printf("Number of lists: %d\n", mtll_size-removed_entries);
    for (int i = 0; i < mtll_size; i++) {
        if (is_in_removed(mtll_list[i])) {
            continue;
        }
        int lsptrpass = 0;
        struct mtll * ptrcopy = mtll_list[i];
        while (ptrcopy != NULL) {
            if (ptrcopy->type == LSPTR) {
                lsptrpass = 1;
            } 
            ptrcopy = ptrcopy->next;
        }

        if (lsptrpass) {
            printf("Nested %d\n", i);
        } else {
            printf("List %d\n", i);
        }
    }
}

// Prints out mtll entries for a specific mtll
void view_x(char ** words) {
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if (words[1] == NULL) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    char * potential_size = words[1];
    if (!is_intable(potential_size)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int index = atoi(potential_size);

        // Return INVALID COMMAND if the size is negative
        if (index < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else if (index >= mtll_size) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        struct mtll * ptr = mtll_list[index];
        
        if (is_in_removed(ptr)) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        if (ptr->type == NONE) {
            printf("\n");
            return;
        }

        if (ptr->next ==  NULL) {
            if (ptr->type != LSPTR) {
                print_data(ptr);
            } else {
                printf("{List %d}", index_of(ptr));
            }
            printf("\n");
        } else {
            while (ptr->next != NULL) {
                if (ptr->type != LSPTR) {
                    print_data(ptr);
                } else {
                    printf("{List %d}", index_of(ptr));
                }
                printf(" -> ");
                ptr = ptr->next;
            }
            if (ptr->type != LSPTR) {
                print_data(ptr);
            } else {
                printf("{List %d}", index_of(ptr));
            }
            printf("\n");
        }
    }
}

// Prints out the type of entries in a given mtll
void type_x(char ** words) {
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if (words[1] == NULL) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    char * potential_size = words[1];
    if (!is_intable(potential_size)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int index = atoi(potential_size);

        // Return INVALID COMMAND if the size is negative
        if (index < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else if (index >= mtll_size) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        struct mtll * ptr = mtll_list[index];
        if (is_in_removed(ptr)) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        if (ptr->next ==  NULL) {
            if (ptr->type != LSPTR) {
                print_data(ptr);
            } else {
                printf("{List %d}", index_of(ptr));
            }
            printf("\n");
        } else {
            while (ptr->next != NULL) {
                print_type(ptr);
                printf(" -> ");
                ptr = ptr->next;
            }
            print_type(ptr);
            printf("\n");
        }
    }
}

// Removes a mtll from mtll_list
void remove_x(char ** words) {
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if (words[1] == NULL) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    char * potential_size = words[1];
    if (!is_intable(potential_size)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int index = atoi(potential_size);

        // Return INVALID COMMAND if the size is negative
        if (index < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else if (index >= mtll_size) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } 
        
        int * x = used_elsewhere(index);
        if (x != NULL) {
            free(x);
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        struct mtll * ptr = mtll_list[index];
        if (is_in_removed(ptr)) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        if (removing_list == NULL) {
            removing_list = (struct mtll **)malloc(sizeof(struct mtll *) * 1);
            removing_list[0] = NULL;
            if (removing_list == NULL) {
                printf("Memory allocation failed.\n");
                free(removing_list);
                return;
            }
        } else {
            removing_list = (struct mtll **)realloc(removing_list, sizeof(struct mtll *) * (removed_entries + 1));
            removing_list[removed_entries] = NULL;
            if (removing_list == NULL) {
                printf("Memory allocation failed.\n");
                free(removing_list);
                return;
            }
        }

        removing_list[removed_entries] = ptr;
        removed_entries += 1;
        printf("List %d has been removed.\n\n", index);
        view_all();

    }
}

// Inserts an element into an mtll
void insert_x(char ** words) {
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if ((words[1] == NULL) || (words[2] == NULL)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    int false_space = 1;
    if (words[3] == NULL) {
        if (valid_command) {
            words[3] = strdup("");
            false_space = 0;
        }
    }
    char * mtll_ist_index = words[1];
    char * list_index = words[2];
    if (!is_intable(mtll_ist_index) || !is_intable(list_index)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int mtll_index = atoi(mtll_ist_index);
        int index = atoi(list_index);

        // Return INVALID COMMAND if the size is negative
        if (mtll_index < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else if (mtll_index >= mtll_size) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        if ((strcmp(words[3], "") == 0) && false_space) {
            words[4] = strdup("");
        }

        // This creates an element for the rest of the words in the array
        int start_index = 3;
        char combined_word[MAX_WORD_LENGTH * MAX_WORDS]; 
        combined_word[0] = '\0';

        // Add the words to the combined_word with spaces in between
        for (int i = start_index; words[i] != NULL; i++) {
            strcat(combined_word, words[i]); 
            strcat(combined_word, " "); 
        }

        // Remove the trailing space, if any
        if (strlen(combined_word) > 0) {
            combined_word[strlen(combined_word) - 1] = '\0';
        }

        // Attempt to insert the element to the list
        struct mtll * candidate = mtll_insert(mtll_list[mtll_index], index, combined_word, mtll_list, mtll_size);

        if (candidate != NULL) {
            int counter = 0;
            int *indices = used_elsewhere(mtll_index);
            if (indices == NULL) {
                mtll_list[mtll_index] = candidate;
                new_view(mtll_list[mtll_index], mtll_index);
                return;
            }
            struct mtll *ptr;
            while (indices[counter] != -1) {
                int i = indices[counter];
                ptr = mtll_list[i];

                while (ptr != NULL) {
                    if (ptr->type == LSPTR) {
                        if (ptr->entry.mtllptr == mtll_list[mtll_index]) {
                            ptr->entry.mtllptr = candidate;
                        }
                    }
                    ptr = ptr->next;
                }

                counter++;
            }
            mtll_list[mtll_index] = candidate;
            new_view(mtll_list[mtll_index], mtll_index);
            free(indices);
        } else {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }
    }
}

// Deletes an element from a mtll
void delete_x(char ** words) {
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if ((words[1] == NULL) || (words[2] == NULL) || (words[3] != NULL)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    char * mtll_ist_index = words[1];
    char * list_index = words[2];
    if (!is_intable(mtll_ist_index) || !is_intable(list_index)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int mtll_index = atoi(mtll_ist_index);
        int index = atoi(list_index);

        // Return INVALID COMMAND if the size is negative
        if (mtll_index < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else if (mtll_index >= mtll_size) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        struct mtll * candidate = mtll_delete(mtll_list[mtll_index], index);

        if (candidate != NULL) {
            int counter = 0;
            int *indices = used_elsewhere(mtll_index);
            if (indices == NULL) {
                mtll_list[mtll_index] = candidate;
                new_view(mtll_list[mtll_index], mtll_index);
                return;
            }
            struct mtll *ptr;
            while (indices[counter] != -1) {
                int i = indices[counter];
                ptr = mtll_list[i];

                while (ptr != NULL) {
                    if (ptr->type == LSPTR) {
                        if (ptr->entry.mtllptr == mtll_list[mtll_index]) {
                            ptr->entry.mtllptr = candidate;
                        }
                    }
                    ptr = ptr->next;
                }

                counter++;
            }
            mtll_list[mtll_index] = candidate;
            new_view(mtll_list[mtll_index], mtll_index);
            free(indices);
        } else {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }
    }
}

// Views an mtll's elements and expands upon nested mtll elements
void view_nested(char ** words) {
    // Check that the second word in the command is an integer and is present, 
    //if not return INVALID COMMAND
    if (words[1] == NULL) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    }
    char * potential_size = words[1];
    if (!is_intable(potential_size)) {
        printf("INVALID COMMAND: %s\n", words[0]);
        return;
    } else {
        int index = atoi(potential_size);

        // Return INVALID COMMAND if the size is negative
        if (index < 0) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        } else if (index >= mtll_size) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        struct mtll * ptr = mtll_list[index];
        
        if (is_in_removed(ptr)) {
            printf("INVALID COMMAND: %s\n", words[0]);
            return;
        }

        if (ptr->type == NONE) {
            printf("\n");
            return;
        }

        if (ptr->next ==  NULL) {
            if (ptr->type != LSPTR) {
                print_data(ptr);
            } else {
                printf("{List %d}", index_of(ptr));
            }
            printf("\n");
        } else {
            while (ptr->next != NULL) {
                print_data(ptr);
                printf(" -> ");
                ptr = ptr->next;
            }
            print_data(ptr);
            printf("\n");
        }
    }
}

// Cleans up the allocated memory for mtll elements and the mtll and removing lists themselves
void cleanse_memory() {
    if (mtll_list == NULL) {
        return;
    } else {
        for (int i = 0; i < mtll_size; i++) {
            mtll_free(mtll_list[i]);
        }
        free(mtll_list);  // Free the list of pointers
        
        free(removing_list);
    }
}

// Switches values between -1 -> 5 depending on the string received
//and activates the corresponding method. Values are respective to:
//exit, invalid, new, view all, view, type, remove.
int activate_command(char ** words) {
    // Return if nothing was read
    if (words[0] == NULL) {
        return -1;
    }

    // If "NEW" is received, add the element to the multilink list
    if (strcmp(words[0], "NEW") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: NEW\n");
            return 0;
        }
        add_mtll(words);
        return 1;
    } 
    
    // If "VIEW" is received...
    else if (strcmp(words[0], "VIEW") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: VIEW\n");
            return 0;
        }
        // If "ALL" is received right after, view all
        if (strcmp(words[1], "ALL") == 0) {
            view_all();
            return 2;
        } 
        
        // If anything else is received, attempt to view a particular element
        else {
            view_x(words);
            return 3;
        }
    } 
    
    // If "TYPE" is received, attempt to view the type of an element
    else if (strcmp(words[0], "TYPE") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: TYPE\n");
            return 0;
        }

        type_x(words);
        return 4;
    } 
    
    // If "REMOVE" is received, attempt to remove an element
    else if (strcmp(words[0], "REMOVE") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: REMOVE\n");
            return 0;
        }

        remove_x(words);
        return 5;
    } 

    // If "INSERT" is received, attempt to insert an element
    else if (strcmp(words[0], "INSERT") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: INSERT\n");
            return 0;
        }

        insert_x(words);
        return 6;
    } 

    // If "DELETE" is received, attempt to delete an element
    else if (strcmp(words[0], "DELETE") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: DELETE\n");
            return 0;
        }

        delete_x(words);
        return 7;
    } 

    // If "VIEW-NESTED" is received, attempt to remove an element
    else if (strcmp(words[0], "VIEW-NESTED") == 0) {

        // Check a second argument was given
        if (words[1] == NULL) {
            printf("INVALID COMMAND: DELETE\n");
            return 0;
        }

        view_nested(words);
        return 8;
    } 
    
    // If "EXIT" is received, cleanse the memory and exit the program
    else if (strcmp(words[0], "EXIT") == 0) {
        return -1;
    } 
    
    // If anything else is received, indicate it is an invalid command
    else {
        for (int i = 0; words[i] != NULL; i++) {
            printf("INVALID COMMAND: INPUT\n");
        }
        return 0;
    }
}

int main(int argc, char** argv) {
    int x = 0;
    while (x != -1) {
        fflush(stdout);

        char **words = split_line_into_words();
        // If EOF, exit the loop
        if (words == NULL) {
            x = -1;  // Set exit code to -1 for EOF
        } else if (!valid_command && words[1] != NULL) {
            if (strcmp(words[0], "INSERT") == 0) {
                printf("INVALID COMMAND: INSERT\n");
            } else if (strcmp(words[0], "DELETE") == 0) {
                printf("INVALID COMMAND: DELETE\n");
            } else {
                printf("INVALID COMMAND: INPUT\n");
            }
            x = 0;
        } else {
            // Otherwise, get the command for the word used
            x = activate_command(words);
        }

        for (int i = 0; words[i] != NULL; i++) {
            free(words[i]);  // Free memory allocated for each word
        }

        // Free memory allocated for the array of words
        free(words);
    }

    cleanse_memory();
}
