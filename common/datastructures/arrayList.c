#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
typedef struct {
    unsigned int length;
    unsigned int capacity;
    size_t elementSize;
    bool freeOnRemove;
    char *items; // using char because it makes memory management simpler (less casting)
} ArrayList;

void alResizeList(ArrayList *list, unsigned int newCapacity) {
    if (list->length > newCapacity) {
        if (list->freeOnRemove) {
            for (int i = newCapacity; i < list->length; i++) {
                free(((void **) list->items)[i]);
            }
        }
        list->length = newCapacity;
    }
    list->items = realloc(list->items, list->elementSize*newCapacity);
    list->capacity = newCapacity;
}
void alFree(ArrayList *list) {
    alResizeList(list, 0);
    free(list->items);
    free(list);
};

void *alGet(ArrayList *list, unsigned int index) {
    // out of bounds
    if (index < 0 || index >= list->length) return NULL;

    return &(list->items)[index*list->elementSize];
}

void alAppend(ArrayList *list, void *item) {

    if (list->length == list->capacity) {
        // resize first
        alResizeList(list, list->capacity*2 + 1);
        //                                 ^ in case it was zero
    }
    memcpy(list->items + list->length++ * list->elementSize, item, list->elementSize);
}
void alConcat(ArrayList *list, ArrayList *items) {
    if (list->elementSize != items->elementSize) return; // invalid
    if (list->length+items->length > list->capacity) {
        // resize first
        alResizeList(list, list->capacity*2 + items->length);
        
    }
    memcpy(list->items + list->length * list->elementSize, items->items, list->elementSize*items->length);
    list->length += items->length;
}
void alConcatAndFree(ArrayList *list, ArrayList *items) {
    alConcat(list, items);
    alFree(items);
}


void *alRemove(ArrayList *list, unsigned int index) {
    // out of bounds
    if (index < 0 || index >= list->length) return NULL;
    
    void *item = NULL;
    if (list->freeOnRemove) {
        void *item = ((void **) list->items)[index];
    }

    // shift everything back
    for (int i = index+1; i < list->length; i++) {
        memcpy(list->items + (i-1) * list->elementSize, list->items + i * list->elementSize, list->elementSize);
    }
    list->length--;

    return item;
}

void *alReplace(ArrayList *list, unsigned int index, void *newItem) {
    // out of bounds
    if (index < 0 || index >= list->length) return NULL;

    void *oldItem = NULL;
    if (list->freeOnRemove) {
        void *oldItem = ((void **) list->items)[index];
    }

    memcpy(list->items + index * list->elementSize, newItem, list->elementSize);
    return oldItem;
}

void alReplaceAndFree(ArrayList *list, unsigned int index, void *newItem) {
    free(alReplace(list, index, newItem));
}

void alRemoveAndFree(ArrayList *list, unsigned int index) {
    free(alRemove(list, index));
}

void alClear(ArrayList *list) {
    alResizeList(list, 0);
}

ArrayList *newArrayList(size_t elementSize) {
    ArrayList *list = malloc(sizeof(ArrayList));
    list->length = 0;
    list->items = NULL;
    list->capacity = 0;
    list->elementSize = elementSize;
    list->freeOnRemove = false;
    return list;
}

ArrayList *newArrayListWithCapacity(size_t elementSize, unsigned int initialCapacity) {
    ArrayList *list = malloc(sizeof(ArrayList));
    list->length = 0;
    list->items = NULL;
    list->capacity = 0;
    list->elementSize = elementSize;
    list->freeOnRemove = false;
    alResizeList(list, initialCapacity);
    return list;
}



