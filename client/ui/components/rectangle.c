#include "../component.h"
#include "../utils.h"
#include <curses.h>
#include <stdlib.h>
#include <stdio.h>

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    unsigned char color;
} RectangleComponent;

// this function will not be exported in the header,
// so it can only be directly accessed by this file.
// because of this, this render function will only be
// applied to rectangles. this means, outside of 
// malicious usage, the downcasting in this function 
// is safe.
void renderRect(Component *component, BoundingBox *bbox) {
    RectangleComponent *rect = (RectangleComponent*) component;
    // char txt[20];
    // sprintf(txt, "p %d %d", bbox->topLeft.x, bbox->topLeft.y);
    // mvaddstr(0, 0, txt);
    
    attrset(COLOR_PAIR(rect->color));
    for (int x = 0; x < bbox->size.x; x++) {
        for (int y = 0; y < bbox->size.y; y++) {
            mvaddch(y + bbox->topLeft.y, x + bbox->topLeft.x, ' ');
        }
    }
    attrset(A_NORMAL);
}

RectangleComponent *newRectangle() {
    RectangleComponent *rect = malloc(sizeof(RectangleComponent));
    rect->component.anchor = zeroAnchor;
    rect->component.render = renderRect;
    return rect;
}