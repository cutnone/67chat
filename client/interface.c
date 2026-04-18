

#include <ncurses.h>
#include <libc.h>
#include "ui/component.h"
#include "ui/components/rectangle.h"


Component *screenComponent = NULL;

void init() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    nonl();
    noecho();

    if (has_colors()) {
        start_color();
        
        /*
        * Simple color assignment, often all we need.  Color pair 0 cannot
        * be redefined.  This example uses the same value for the color
        * pair as for the foreground color, though of course that is not
        * necessary:
        */
       init_pair(1, COLOR_BLACK, COLOR_RED);
       init_pair(2, COLOR_BLACK, COLOR_GREEN);
       init_pair(3, COLOR_BLACK, COLOR_YELLOW);
       init_pair(4, COLOR_BLACK, COLOR_BLUE);
       init_pair(5, COLOR_BLACK, COLOR_CYAN);
       init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
       init_pair(7, COLOR_BLACK, COLOR_WHITE);
    }

    Rectangle *rect = newRectangle();
    rect->color = 2;
    rect->component.anchor.size.absX = 35;
    rect->component.anchor.size.yType = VEC_RELATIVE;
    rect->component.anchor.size.relY = 1;

    rect->component.anchor.position.xType = VEC_RELATIVE;
    rect->component.anchor.position.yType = VEC_RELATIVE;
    rect->component.anchor.position.relX = 0.5;
    rect->component.anchor.position.relY = 0.5;

    rect->component.anchor.origin.relX = 0.5;
    rect->component.anchor.origin.relY = 0.5;
    screenComponent = (Component*) rect;
}

void rerender() {
    erase();
    int maxY = getmaxy(curscr), maxX = getmaxx(curscr);
    BoundingBox screenBox = {
        .topLeft = {
            .x = 0,
            .y = 0,
        },
        .size = {
            .x = maxX,
            .y = maxY,
        },
        
    };
    if (screenComponent != NULL) {
        BoundingBox *applied = generateChildBoundingBox(&screenBox, &screenComponent->anchor);
        screenComponent->render(screenComponent, applied);
    }
    refresh();
}

void checkKeystrokes() {
    int x, y;
    getyx(curscr, y, x);
    int c = getch();
    switch (c) {
        case KEY_UP:
            move(y-1, x);
            break;
        case KEY_DOWN:
            move(y+1, x);
            break;
        case KEY_LEFT:
            move(y, x-1);
            break;
        case KEY_RIGHT:
            move(y, x+1);
            break;
        case KEY_BACKSPACE:
        case 127:
            mvaddch(y, x-1, ' ');
            move(y, x-1);
            break;
        case '\r':
            move(y+1, x);
            break;
        default:
            // printw("%d", c);
            addch(c);
            break;
    }
    rerender();
}

void cleanup() {
    endwin();
}

void resize() {
    // these lines are from claude
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);  // ask kernel for new size
    printf("%d %d\n", ws.ws_row, ws.ws_col);
    resizeterm(ws.ws_row, ws.ws_col);
    endwin();
    refresh();
    rerender();
}