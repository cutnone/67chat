
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
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
void sbClip(StringBuilder *sb, unsigned int n) {
    if (sb->length < n) {
        sb->data[0] = '\0';
        sb->length = 0;
    } else {
        sb->data[sb->length-n] = '\0';
        sb->length = sb->length-n;
    }
}
void sbRemove(StringBuilder *sb, unsigned int index) {
    // out of bounds
    if (index < 0 || index >= sb->length) return;

    // shift everything back ("<=" to include '\0')
    for (int i = index+1; i <= sb->length; i++) {
        sb->data[i-1] = sb->data[i];
    }
    sb->length--;
}
void sbInsertC(StringBuilder *sb, unsigned int index, char src) {
    // out of bounds
    if (index < 0 || index > sb->length) return;

    // resize check
    if (sb->length + 1 > sb->capacity) {
        // 1.5x the cap, +1 in case it rounds to the same amount
        sbResize(sb, sb->capacity + sb->capacity/2 + 1);
    }
    // return;
    // shift everything forward
    for (int i = sb->length; i >= index && i <= sb->length; i--) {
        (sb->data)[i+1] = (sb->data)[i];
    }
    sb->length++;
    (sb->data)[index] = src;
}


StringBuilder *newStringBuilder() {
    StringBuilder *sb = malloc(sizeof(StringBuilder));
    sb->capacity = 0;
    sb->length = 0;
    sb->data = malloc(1);
    (sb->data)[0] = '\0';
    return sb;
}
