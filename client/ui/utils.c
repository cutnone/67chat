#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

typedef struct {
    int x;
    int y;
} IntVec2;

typedef struct {
    float x;
    float y;
} FloatVec2;

// this pattern suggested by Claude
typedef enum {
    VEC_ABSOLUTE,
    VEC_RELATIVE,
} Vec2Type;
typedef struct {
    Vec2Type xType;
    Vec2Type yType;
    union {
        int absX;
        float relX;
    };
    union {
        int absY;
        float relY;
    };
} GenericVec2;

typedef struct {
    GenericVec2 origin;
    GenericVec2 position;
    GenericVec2 size;
} Anchor;
Anchor zeroAnchor = {
    .origin = {
        .xType = VEC_RELATIVE,
        .yType = VEC_RELATIVE,
        .relX = 0,
        .relY = 0,
    },
    .position = {
        .xType = VEC_ABSOLUTE,
        .yType = VEC_ABSOLUTE,
        .absX = 0,
        .absY = 0,
    },
    .size = {
        .xType = VEC_ABSOLUTE,
        .yType = VEC_ABSOLUTE,
        .absX = 0,
        .absY = 0,
    }
};

typedef struct {
    IntVec2 topLeft;
    IntVec2 size;
} BoundingBox;

IntVec2 *applyGenericVec2(IntVec2 *base, GenericVec2 *vec) {
    IntVec2 *final = malloc(sizeof(IntVec2));
    switch (vec->xType) {
        case VEC_ABSOLUTE:
            final->x = vec->absX;
            break;
        case VEC_RELATIVE:
            final->x = round(((float) base->x) * vec->relX);
            break;
    }
    switch (vec->yType) {
        case VEC_ABSOLUTE:
            final->y = vec->absY;
            FILE *log = fopen("debug.log", "a");
            fprintf(log, "applyingB %d %d %d\n", base->y, vec->absY, final->y);
            fclose(log);
            break;
        case VEC_RELATIVE:
            final->y = round(((float) base->y) * vec->relY);
            break;
    }
    FILE *log = fopen("debug.log", "a");
    fprintf(log, "applying %d %d %d %d\n", base->x, base->y, final->x, final->y);
    fclose(log);
    return final;
}

BoundingBox *generateChildBoundingBox(BoundingBox *outerBox, Anchor *anchor) {
    BoundingBox *finalBox = malloc(sizeof(BoundingBox));
    finalBox->size = *applyGenericVec2(&outerBox->size, &anchor->size);
    IntVec2 computedOrigin = *applyGenericVec2(&finalBox->size, &anchor->origin);
    IntVec2 computedPosition = *applyGenericVec2(&outerBox->size, &anchor->position);
    
    // alter the position according to the anchor
    computedPosition.x -= computedOrigin.x;
    computedPosition.y -= computedOrigin.y;
    
    // go from relative to parent to absolute
    computedPosition.x += outerBox->topLeft.x;
    computedPosition.y += outerBox->topLeft.y;
    
    finalBox->topLeft = computedPosition;

    return finalBox;
}

