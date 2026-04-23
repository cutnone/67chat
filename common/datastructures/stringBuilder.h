#pragma once
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int capacity;
    unsigned int length;
    char *data;
} StringBuilder;

void sbAppend(StringBuilder *sb, char *src, unsigned int srcLen);
void sbAppendC(StringBuilder *sb, char src);
void sbResize(StringBuilder *sb, unsigned int newCapacity);
void sbClear(StringBuilder *sb);
StringBuilder *newStringBuilder();
