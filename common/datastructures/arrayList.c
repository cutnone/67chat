#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

typedef struct {
    unsigned int length;
    unsigned int capacity;
    void **items;
} ArrayList;

void alResizeList(ArrayList *list, unsigned int newCapacity) {
    if (list->length > newCapacity) {
        for (int i = newCapacity; i < list->length; i++) {
            free(list->items[i]);
        }
        list->length = newCapacity;
    }
    list->items = realloc(list->items, sizeof(void*)*newCapacity);
    list->capacity = newCapacity;
}

void *alGet(ArrayList *list, unsigned int index) {
    // out of bounds
    if (index < 0 || index >= list->length) return NULL;

    return list->items[index];
}

void alAppend(ArrayList *list, void *item) {

    if (list->length == list->capacity) {
        // resize first
        alResizeList(list, list->capacity*2 + 1);
        //                                 ^ in case it was zero
    }
    list->items[list->length++] = item;
}

void *alRemove(ArrayList *list, unsigned int index) {
    // out of bounds
    if (index < 0 || index >= list->length) return NULL;
    
    // free deleted item
    void *item = list->items[index];

    // shift everything back
    for (int i = index+1; i < list->length; i++) {
        list->items[i-1] = list->items[i];
    }
    list->length--;

    return item;
}

void *alReplace(ArrayList *list, unsigned int index, void *newItem) {
    // out of bounds
    if (index < 0 || index >= list->length) return NULL;

    void *oldItem = list->items[index];
    list->items[index] = newItem;
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

ArrayList *newArrayList() {
    ArrayList *list = malloc(sizeof(ArrayList));
    list->length = 0;
    list->items = NULL;
    list->capacity = 0;
    return list;
}

ArrayList *newArrayListWithCapacity(unsigned int initialCapacity) {
    ArrayList *list = malloc(sizeof(ArrayList));
    list->length = 0;
    list->items = NULL;
    list->capacity = 0;
    alResizeList(list, initialCapacity);
    return list;
}



