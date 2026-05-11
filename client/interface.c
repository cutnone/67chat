

#include <curses.h>
#include "ui/component.h"
#include "comms.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <termios.h>
#endif
#include "ui/components/rectangle.h"
#include "ui/components/text.h"
#include "ui/components/group.h"
#include "ui/scenes/resizeScene.h"
#include "../common/datastructures/arrayList.h"

Component *screenComponent = NULL;

void init() {
    initscr();
    keypad(stdscr, TRUE);
    cbreak();
    nonl();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
    #ifndef _WIN32
        set_escdelay(25);
    #endif
    if (has_colors()) {
        start_color();
        
        /*
        * Simple color assignment, often all we need.  Color pair 0 cannot
        * be redefined.  This example uses the same value for the color
        * pair as for the foreground color, though of course that is not
        * necessary:
        */
       init_color(8, 200, 200, 200);
       init_color(9, 800, 800, 800);
       init_color(10, 100, 100, 100);
       init_pair(1, COLOR_WHITE, 8);
       init_pair(2, COLOR_BLACK, 9);
       init_pair(3, COLOR_WHITE, 10);
       init_pair(4, COLOR_BLACK, COLOR_BLUE);
       init_pair(5, COLOR_BLACK, COLOR_CYAN);
       init_pair(6, COLOR_BLACK, COLOR_MAGENTA);
       init_pair(7, COLOR_BLACK, COLOR_WHITE);
    }
    
}

void rerender() {
    erase();
    int maxY = getmaxy(curscr), maxX = getmaxx(curscr);
    
    Component *renderThis = screenComponent;
    if (maxX < MIN_WIDTH || maxY < MIN_HEIGHT) {
        renderThis = (Component*) resizeScene;
    }
    BoundingBox screenBox = {
        .topLeft = {
            .x = 0,
            .y = 0,
        },
        .size = {
            .x = maxX,
            .y = maxY,
        },
        .clip = true,
    };
    if (renderThis != NULL) {
        attrset(A_NORMAL); // clear formatting
        BoundingBox *applied = generateChildBoundingBox(&screenBox, &renderThis->anchor);
        renderThis->render(renderThis, applied);
    }
    refresh();
}

void resize() {
    // these lines are from claude
    #ifdef _WIN32
        // CONSOLE_SCREEN_BUFFER_INFO csbi;
        // GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        // int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        // int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        // resize_term(rows, cols);
        resize_term(0, 0);
    #else
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);  // ask kernel for new size
        resizeterm(ws.ws_row, ws.ws_col);
    #endif
    alResizeList(messageCache, getmaxy(curscr));
    // endwin();
    refresh();
    rerender();
}

void checkKeystrokes() {
    int x, y;
    getyx(curscr, y, x);
    int c = getch();
    switch (c) {
        case KEY_RESIZE:
            resize();
            break;
        default:
            if (screenComponent != NULL && screenComponent->receiveInput != NULL) {
                screenComponent->receiveInput(screenComponent, c);
            }
            break;
    }

    rerender();
}

void cleanup() {
    endwin();
    #ifdef _WIN32
        WSACleanup();
    #endif
}
