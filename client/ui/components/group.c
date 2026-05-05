
#include "../component.h"
#include "../utils.h"
#include "rectangle.h"
#include "../../../common/datastructures/arrayList.h"
#include <curses.h>
#include <stdlib.h>
#include <stdio.h>


// these constraints alter how 
// components in the group are positioned.
// VBOX, for example, cuts away vertical space
// as new components are rendered, creating
// the ability to stack components vertically.
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

void renderGroup(Component *component, BoundingBox *box) {
    Group *group = (Group*) component;
    
    if (group->background != NULL) {
        group->background->component.render((Component *) group->background, box);
    }
    
    // apply margin
    box->topLeft.y += group->topMargin;
    box->size.y -= group->topMargin + group->bottomMargin;
    box->topLeft.x += group->leftMargin;
    box->size.x -= group->leftMargin + group->rightMargin;
    
    for (int i = 0; i < group->components->length; i++) {
        Component *innerComp = * (Component **)alGet(group->components, i);
        BoundingBox *innerBox = generateChildBoundingBox(box, &innerComp->anchor);
        // return;
        innerComp->render(innerComp, innerBox);
        
        // bitmask trick i learned but never had a reason to use
        // until now. is nonzero when horizontal constraining is needed.
        if (group->constraint & GROUP_HBOX) {
            int outerCenter = box->topLeft.x + box->size.x/2;
            int innerCenter = innerBox->topLeft.x + innerBox->size.x/2;
            if (innerCenter >= outerCenter) {
                // cut right
                int hdist = innerBox->topLeft.x - box->topLeft.x - group->xPad;
                // make sure it's still in bounds
                if (hdist >= 0 && hdist < box->size.x) {
                    box->size.x = hdist;
                }
            } else {
                // cut left
                int hdist = innerBox->topLeft.x + innerBox->size.x - box->topLeft.x + group->xPad;
                if (hdist >= 0 && hdist < box->size.x) {
                    box->size.x -= hdist;
                    box->topLeft.x += hdist;
                }
            }
        }
        if (group->constraint & GROUP_VBOX) {
            int outerCenter = box->topLeft.y + box->size.y/2;
            int innerCenter = innerBox->topLeft.y + innerBox->size.y/2;
            if (innerCenter >= outerCenter) {
                // cut bottom
                printf("BOMM CUT\n");
                int vdist = innerBox->topLeft.y - box->topLeft.y - group->yPad;
                // make sure it's still in bounds
                if (vdist >= 0 && vdist < box->size.y) {
                    box->size.y = vdist;
                }
            } else {
                printf("TOM CUT\n");
                // cut top
                int vdist = innerBox->topLeft.y + innerBox->size.y - box->topLeft.y + group->yPad;
                if (vdist >= 0 && vdist < box->size.y) {
                    box->size.y -= vdist;
                    box->topLeft.y += vdist;
                }
            }
        }
    }
    
}

void groupReceiveInput(Component * component, int key) {
    Group *group = (Group*) component;
    for (int i = 0; i < group->components->length; i++) {
        Component *innerComp = * (Component **)alGet(group->components, i);
        if (innerComp->receiveInput != NULL) {
            innerComp->receiveInput(innerComp, key);
        }
    }
}

void groupAddBackground(Group *group, unsigned char color) {
    if (group->background == NULL) {
        group->background = newRectangle();
    }
    group->background->color = color;
}

Group *newGroup() {
    Group *g = malloc(sizeof(Group));
    g->components = newArrayList(sizeof(Component *));
    g->component.anchor = zeroAnchor;
    g->constraint = GROUP_FREE;
    g->background = NULL;
    g->xPad = 0;
    g->yPad = 0;
    g->topMargin = 0;
    g->bottomMargin = 0;
    g->leftMargin = 0;
    g->rightMargin = 0;
    g->component.render = renderGroup;
    g->component.receiveInput = groupReceiveInput;
    return g;
}