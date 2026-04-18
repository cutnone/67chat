#pragma once
#include "../component.h"
#include "../utils.h"
#include <ncurses.h>

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    char color;
} Rectangle;

Rectangle *newRectangle();