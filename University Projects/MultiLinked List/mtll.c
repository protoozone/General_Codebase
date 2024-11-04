#include "mtll.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Returns a negative value if the word is not a nestable element, 
//returns the nesting index if it is
int nestable_element(char *word) {
    if (word[0] == '{') {
        
        // Establishing arrays for word reading (max size is 128 bytes according to assignment specs)
        char word2[129];
        int letter_count = 0;
        
        // Add all the integer data to the array word2
        while (word[letter_count + 1] != '}') {
            word2[letter_count] = word[letter_count + 1];
            letter_count++;
        }

        // Add a null terminator
        word2[letter_count] = '\0';
        
        // Returns the size of the value within {}
        int size = atoi(word2);
        if (word2 < 0) {
            return -2;
        }
        return size;
    }
    
    // Return 0 if this fails for any reason
    return -1;
}

// Instantiates an multi linked list
struct mtll *mtll_create(int size) {

    // Null check for invalid size
    if (size <= 0) {
        return NULL;  
    }

    // Allocate memory for the head node
    struct mtll *head = (struct mtll *)malloc(sizeof(struct mtll));  
    if (head == NULL) {
        return NULL;  
    }

    // Initialize temp to head
    struct mtll *temp = head;  
    temp->type = NONE;

    // Create size-1 additional nodes and link them together
    for (int i = 1; i < size; i++) {

        // Allocate memory for the next node
        temp->next = (struct mtll *)malloc(sizeof(struct mtll));  
        if (temp->next == NULL) {
            
            // If memory allocation fails, free the memory and return NULL
            mtll_free(head);
            return NULL;
        }
        // Move temp to the newly created node
        temp = temp->next; 
        temp->type = NONE;
    }
    temp->next = NULL;

    // Return the head of the linked list
    return head;  
}

// Frees the mtll element and connected elements
void mtll_free(struct mtll * head) {
    struct mtll * x = head;
    struct mtll * temp = x;
    while (x != NULL) {
        temp = x;
        x = x->next;
        free(temp);
    }
}

// Adds element data to an mtll body
int mtll_add(struct mtll * head, int size, struct mtll ** mtll_list, int mtll_size) {

    // Initialise the head and assume the data was added correctly
    struct mtll * temp = head;
    int added_successfully = 1;

    // For the amount of elements to be added...
    for (int i = 0; i < size; i++) {

        // Get the line information
        char line[129];
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return -1;
        }

        // Clean up the line information
        if (line[strlen(line) - 1] != '\n') {
            int c;

            // Flush input stream
            while ((c = getchar()) != '\n' && c != EOF) {}
            
            return -1;
        }

        line[strcspn(line, "\n")] = 0;

        // Initialise values for if the element is an int or float
        int intValue;
        float floatValue;

        // If the line has a letter somewhere...
        int hasLetter = 0;
        int dotCounter = 0;
        int eCounter = 0;

        // Check if the information of the element indicates if it would be a string or char
        for (int j = 0; line[j] != '\0'; j++) {
            if (line[j] == '.' && j != 0) {
                dotCounter += 1;
                continue;
            }
            if ((line[j] == 'e' && j != 0) || (line[j] == 'E' && j != 0)) {
                dotCounter += 1;
                continue;
            }
            if (line[j] == '-') {
                continue;
            }
            if (!isdigit(line[j])) {
                hasLetter = 1;
                break;
            } 
        }

        // Then skip the int/float section
        if (!hasLetter && dotCounter < 2 && eCounter < 2) {
            // If we can parse it as an integer...
            if (sscanf(line, "%d", &intValue) == 1) {
                // Check if there is a decimal point or exponent
                if (strchr(line, '.') != NULL || strchr(line, 'e') != NULL || strchr(line, 'E') != NULL) {
                    // Add the input to the list as a float
                    if (sscanf(line, "%f", &floatValue) == 1) {
                        temp->entry.flt = floatValue;
                        temp->type = FLT;
                    } else {
                        // If this doesn't work, add as an unknown
                        printf("Invalid float input\n");
                        temp->type = NONE;
                    }
                } else {
                    // If there is no decimal or exponent, add as an int
                    temp->entry.num = intValue;
                    temp->type = INT;
                }
            } else {
                // If there is no letter and a parse is nonsensical, add as nothing
                strncpy(temp->entry.sentence, "", sizeof(temp->entry.sentence) - 1);
                temp->entry.sentence[sizeof(temp->entry.sentence) - 1] = '\0';
                temp->type = STR;
            }
        }
        
        // Check if the lettered input is a single size, add as char
        else if (isprint(line[0]) && !iscntrl(line[0]) && strlen(line) == 1) {  
            temp->entry.letter = line[0];
            temp->type = CHAR;
        }
        // Otherwise, add as string
        else {

            // First check if we can nest it as a nested list
            int x = nestable_element(line);
            if (x < 0) {

                // If not, just add as a string
                strncpy(temp->entry.sentence, line, sizeof(temp->entry.sentence) - 1);
                temp->entry.sentence[sizeof(temp->entry.sentence) - 1] = '\0';
                temp->type = STR;
            } else {

                // Otherwise, return 0 if the nest index is out of range
                if (x >= mtll_size) {
                    temp->type = NONE;
                    return 0;
                }

                // Check if the indexed list does not already have a nested list
                struct mtll * candidate = mtll_list[x];
                int check = 1;
                struct mtll * ptr = candidate;
                while (ptr != NULL) {
                    if (ptr->type == LSPTR) {
                        check = 0;
                    }
                    ptr = ptr->next;
                }

                // If not, link the lists together
                if (check || candidate == NULL) {
                    temp->entry.mtllptr = mtll_list[x];
                    temp->type = LSPTR;
                } else {

                    // Otherwise indicate this could not be completed
                    temp->type = NONE;
                    return 0;
                }
            }
        }
        // Move to the next node
        temp = temp->next;
    }
    // Successfully processed all lines
    return added_successfully;
}

// Inserts an element into the mtll
struct mtll * mtll_insert (struct mtll * head, int index, char * elem, struct mtll ** mtll_list, int mtll_size) {
    
    // Allocate memory for the head node
    struct mtll *new_elem = (struct mtll *)malloc(sizeof(struct mtll)); 
    if (new_elem == NULL) {
        return NULL; 
    }

    // Clean element data and assume type is NONE for now
    new_elem->type = NONE;

    elem[strcspn(elem, "\n")] = 0;

    // Same checks in mtll_add
    int intValue;
    float floatValue;

    // If the line has a letter somewhere...
    int hasLetter = 0;
    int dotCounter = 0;
    int eCounter = 0;

    for (int j = 0; elem[j] != '\0'; j++) {
        if (elem[j] == '.' && j != 0) {
            dotCounter += 1;
            continue;
        }
        if ((elem[j] == 'e' && j != 0) || (elem[j] == 'E' && j != 0)) {
            dotCounter += 1;
            continue;
        }
        if (elem[j] == '-') {
            continue;
        }
        if (!isdigit(elem[j])) {
            hasLetter = 1;
            break;
        } 
    }

    // Then skip the int/float section
    if (!hasLetter && dotCounter < 2 && eCounter < 2) {
        // If we can parse it as an integer...
        if (sscanf(elem, "%d", &intValue) == 1) {
            // Check if there is a decimal point or exponent
            if (strchr(elem, '.') != NULL || strchr(elem, 'e') != NULL || strchr(elem, 'E') != NULL) {
                // Add the input to the list as a float
                if (sscanf(elem, "%f", &floatValue) == 1) {
                    new_elem->entry.flt = floatValue;
                    new_elem->type = FLT;
                } else {
                    // If this doesn't work, add as an unknown
                    printf("Invalid float input\n");
                    new_elem->type = NONE;
                }
            } else {
                // If there is no decimal or exponent, add as an int
                new_elem->entry.num = intValue;
                new_elem->type = INT;
            }
        } else {
            // If there is no letter and a parse is nonsensical, add as nothing
            strncpy(new_elem->entry.sentence, "", sizeof(new_elem->entry.sentence) - 1);
            new_elem->entry.sentence[sizeof(new_elem->entry.sentence) - 1] = '\0';
            new_elem->type = STR;
        }
    }
    
    // Check if the lettered input is a single size, add as char
    else if (isprint(elem[0]) && !iscntrl(elem[0]) && strlen(elem) == 1) {  
        new_elem->entry.letter = elem[0];
        new_elem->type = CHAR;
    }
    // Otherwise, add as string or lsptr depending on format
    else {
        int x = nestable_element(elem);
        
        // Add as string if not nestable
        if (x < 0) {
            strncpy(new_elem->entry.sentence, elem, sizeof(new_elem->entry.sentence) - 1);
            new_elem->entry.sentence[sizeof(new_elem->entry.sentence) - 1] = '\0';
            new_elem->type = STR;
        } else {
            
            // Add as nest list otherwise
            if (x >= mtll_size) {
                free(new_elem);
                return NULL;
            }

            struct mtll * candidate = mtll_list[x];
            int check = 1;
            struct mtll * ptr = candidate;
            while (ptr != NULL) {
                if (ptr->type == LSPTR) {
                    check = 0;
                }
                ptr = ptr->next;
            }

            if (check) {
                new_elem->entry.mtllptr = mtll_list[x];
                new_elem->type = LSPTR;
            }   else {
                free(new_elem);
                return NULL;
            }
        }
    }

    // Find the size of the list
    int size = 0;
    struct mtll * countmtll = head;
    new_elem->next = NULL;
    while (countmtll != NULL) {
        countmtll = countmtll -> next;
        size += 1;
    }

    // If the index is negative, we can set the head to the element if there
    //is no list data, or wrap the index back to a positive one with some arithmetic
    if ((index < 0) && (head->type != NONE)) {
        if (index%(size+1) != 0) {
            index = (index%(size+1)) + (size+1);
        } else {
            index = 0;
        }
    }

    if (index <= 0) {
        if (head->type == NONE) {
            free(head);
            return new_elem;
        }
        new_elem->next = head;
        return new_elem;
    }

    // Can't add data if there is no data
    if ((index == 1) && (head->type == NONE)) {
        return NULL;
    }

    struct mtll *prev = head;
    for (int i = 1; i < index; i++) {
        prev = prev->next;
        if (prev == NULL) {
            return NULL;
        }
    }

    // If the index is greater than one, make the prev point to current
    //make current point to next
    new_elem->next = prev->next;
    prev->next = new_elem;
    return head;
}

// Deletes an element from the mtll structure
struct mtll * mtll_delete (struct mtll * head, int index) {

    // Find the size and wrap like insert (size not + 1 since)
    //there are only size elements to delete and not size+1 positions
    //to place
    struct mtll * temp = head;

    int size = 0;
    struct mtll * countmtll = head;
    while (countmtll != NULL) {
        countmtll = countmtll -> next;
        size += 1;
    }

    if (index < 0) {
        if (index%(size) != 0) {
            index = (index%(size)) + (size);
        } else {
            index = 0;
        }
    }
    
    if (index == 0) {

        // If there is no data, can't remove
        if (head->type == NONE) {
            return NULL;
        }

        if (head->next != NULL) {
            head = temp->next;
            free(temp);
            return head;
        }

        // If only one element, remove it
        else {
            head->type = NONE;
            return head;
        }
    }

    // Otherwise, update the previous node's next node to the one after the next node
    struct mtll * prev = NULL;
    for (int i = 0; i < index; i++) {
        prev = temp;
        temp = temp->next;
        if (temp == NULL) {
            return NULL;
        }
    }

    prev->next = temp->next;

    // And free the node in between
    free(temp);
    return head;
}
