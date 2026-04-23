
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    unsigned int capacity;
    unsigned int length;
    char *data;
} StringBuilder;

void sbResize(StringBuilder *sb, unsigned int newCapacity) {
    bool shrunk = false;
    if (sb->length > newCapacity) {
        sb->length = newCapacity;
        shrunk = true;
    }
    sb->data = realloc(sb->data, sizeof(char)*(newCapacity + 1));
    if (shrunk) {
        sb->data[newCapacity] = '\0';
    }
    sb->capacity = newCapacity;
}
void sbClear(StringBuilder *sb) {
    sbResize(sb, 0);
}

void sbAppend(StringBuilder *sb, char *src, unsigned int srcLen) {
    // out of bounds
    if (srcLen + sb->length > sb->capacity) {
        // make room for the new string and 1.5x the cap.
        sbResize(sb, sb->capacity + srcLen + sb->capacity/2);
    }
    strcat(sb->data, src);
    sb->length += srcLen;
}
void sbAppendC(StringBuilder *sb, char src) {
    // out of bounds
    if (sb->length + 1 > sb->capacity) {
        // 1.5x the cap, +1 in case it rounds to the same amount
        sbResize(sb, sb->capacity + sb->capacity/2 + 1);
    }
    (sb->data)[sb->length] = src;
    (sb->data)[sb->length+1] = '\0';
    sb->length++;
}

StringBuilder *newStringBuilder() {
    StringBuilder *sb = malloc(sizeof(StringBuilder));
    sb->capacity = 0;
    sb->length = 0;
    sb->data = malloc(1);
    (sb->data)[0] = '\0';
    return sb;
}
