#pragma once
#include <stdlib.h>
#include <stdarg.h>

typedef struct {
    unsigned int length;
    unsigned int capacity;
    void **items;
} ArrayList;

void alResizeList(ArrayList *list, unsigned int newCapacity);

void *alGet(ArrayList *list, unsigned int index);

void alAppend(ArrayList *list, void *item);

void *alRemove(ArrayList *list, unsigned int index);

void *alReplace(ArrayList *list, unsigned int index, void *newItem);

void alReplaceAndFree(ArrayList *list, unsigned int index, void *newItem);

void alRemoveAndFree(ArrayList *list, unsigned int index);

void alClear(ArrayList *list);

ArrayList *newArrayList();

ArrayList *newArrayListWithCapacity(unsigned int initialCapacity);



