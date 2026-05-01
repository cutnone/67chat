#pragma once
#include "../component.h"
#include "../utils.h"

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    unsigned char color;
} RectangleComponent;

RectangleComponent *newRectangle();