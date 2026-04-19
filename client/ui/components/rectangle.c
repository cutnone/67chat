#include "../component.h"
#include "../utils.h"
#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

// got this "inheritance" pattern from Claude
typedef struct {
    Component component;
    char color;
} Rectangle;

// this function will not be exported in the header,
// so it can only be directly accessed by this file.
// because of this, this render function will only be
// applied to rectangles. this means, outside of 
// malicious usage, the downcasting in this function 
// is safe.
void renderRect(Component *component, BoundingBox *bbox) {
    Rectangle *rect = (Rectangle*) component;
    char txt[20];
    sprintf(txt, "p %d %d", bbox->topLeft.x, bbox->topLeft.y);
    mvaddstr(0, 0, txt);
    
    attron(COLOR_PAIR(rect->color));
    for (int x = 0; x < bbox->size.x; x++) {
        for (int y = 0; y < bbox->size.y; y++) {
            mvaddch(y + bbox->topLeft.y, x + bbox->topLeft.x, ' ');
        }
    }
    attroff(COLOR_PAIR(rect->color));
}

Rectangle *newRectangle() {
    Rectangle *rect = malloc(sizeof(Rectangle));
    rect->component.anchor = zeroAnchor;
    rect->component.render = renderRect;
    return rect;
}