
#include "../component.h"
#include "../utils.h"
#include "../../../common/datastructures/arrayList.h"
#include <ncurses.h>
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
    ArrayList components;
} Group;

void renderGroup(Component *component, BoundingBox *box) {
    Group *group = (Group*) component;
    for (int i = 0; i < group->components.length; i++) {
        Component *innerComp = alGet(&group->components, i);
        BoundingBox *innerBox = generateChildBoundingBox(box, &innerComp->anchor);
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
                int vdist = innerBox->topLeft.y - box->topLeft.y - group->yPad;
                // make sure it's still in bounds
                if (vdist >= 0 && vdist < box->size.y) {
                    box->size.y = vdist;
                }
            } else {
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

Group *newGroup() {
    Group *g = malloc(sizeof(Group));
    g->components.length = 0;
    g->components.items = NULL;
    g->components.capacity = 0;
    g->component.anchor = zeroAnchor;
    g->constraint = GROUP_FREE;
    g->xPad = 0;
    g->yPad = 0;
    g->component.render = renderGroup;
    return g;
}