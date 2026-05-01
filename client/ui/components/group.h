
#pragma once
#include "../component.h"
#include "../utils.h"
#include "rectangle.h"
#include "../../../common/datastructures/arrayList.h"
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
    int topMargin;
    int bottomMargin;
    int leftMargin;
    int rightMargin;
    ArrayList *components;
    RectangleComponent *background;
} Group;

void groupAddBackground(Group *group, unsigned char color);

Group *newGroup();