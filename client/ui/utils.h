#pragma once
#include <math.h>

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
Anchor zeroAnchor;

typedef struct {
    IntVec2 topLeft;
    IntVec2 size;
} BoundingBox;

IntVec2 *applyGenericVec2(IntVec2 *base, GenericVec2 *vec);

BoundingBox *generateChildBoundingBox(BoundingBox *outerBox, Anchor *anchor);