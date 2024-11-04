#ifndef MTLL_H
#define MTLL_H
// size_t = 129;

// Define an enum to speed up future prints
typedef enum {
    INT,
    FLT,
    CHAR,
    STR,
    LSPTR,
    NONE
} Object_Type;

// Define the list entry union
union Mtll_object {
    int num;
    float flt;
    char letter;
    char sentence[129]; // 129 is selected for 128 byte length + newline
    struct mtll * mtllptr; // Pointer to a nested list within this list
};

struct mtll {
    union Mtll_object entry;
    Object_Type type; 
    struct mtll* next;
};

// A few suggested function prototypes:
// Feel free to change or improve these as you need.

extern struct mtll *mtll_create(int);

extern int mtll_add(struct mtll *, int, struct mtll **, int);

extern void mtll_free(struct mtll *);

extern struct mtll * mtll_insert(struct mtll *, int, char *, struct mtll **, int);

extern struct mtll * mtll_delete(struct mtll *, int);

extern void mtll_remove(struct mtll **, int, struct mtll *);

// You will likely need other functions for Parts 2 and 3,
// And for general input / output management


#endif
