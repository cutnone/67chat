#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    unsigned int length;
    unsigned int capacity;
    size_t elementSize;
    bool freeOnRemove;
    char *items; // using char because it makes memory management simpler (less casting)
} ArrayList;

void alResizeList(ArrayList *list, unsigned int newCapacity);

void *alGet(ArrayList *list, unsigned int index);
void alAppend(ArrayList *list, void *item);

void *alRemove(ArrayList *list, unsigned int index);

void *alReplace(ArrayList *list, unsigned int index, void *newItem);

void alReplaceAndFree(ArrayList *list, unsigned int index, void *newItem);

void alRemoveAndFree(ArrayList *list, unsigned int index);

void alClear(ArrayList *list);

ArrayList *newArrayList(size_t elementSize);

ArrayList *newArrayListWithCapacity(size_t elementSize, unsigned int initialCapacity);
