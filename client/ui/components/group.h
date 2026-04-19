
#pragma once
#include "../component.h"
#include "../utils.h"
#include "../../../common/datastructures/arrayList.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    GROUP_FREE = 0b00,
    GROUP_HBOX = 0b10,
    GROUP_VBOX = 0b01,
    GROUP_BOTH = 0b11,
} GroupConstraint;

typedef struct {
    Component component;
    GroupConstraint constraint;
    int xPad;
    int yPad;
    ArrayList components;
} Group;

Group *newGroup();