

#include <curses.h>
#include "ui/component.h"
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
#include "../common/datastructures/arrayList.h"

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
    

    Group *g = newGroup();
    g->constraint = GROUP_VBOX;
    g->component.anchor.size.xType = VEC_RELATIVE;
    g->component.anchor.size.yType = VEC_RELATIVE;
    g->component.anchor.size.relX = 1;
    g->component.anchor.size.relY = 1;
    g->yPad = 1;
    RectangleComponent *rect = newRectangle();
    rect->color = 1;
    rect->component.anchor.size.absX = 35;
    // rect->component.anchor.size.yType = VEC_RELATIVE;
    rect->component.anchor.size.absY = 10;
    
    rect->component.anchor.position.xType = VEC_RELATIVE;
    rect->component.anchor.position.yType = VEC_RELATIVE;
    rect->component.anchor.position.relX = 0.5;
    rect->component.anchor.position.relY = 0;
    
    rect->component.anchor.origin.relX = 0.5;
    rect->component.anchor.origin.relY = 0;
    TextComponent *rect2 = newTextComponent();
    ArrayList *instructions = stringToInstructions("Hey guys, did you know that in terms of male human and female Pokemon breeding, Vaporeon is the most compatible Pokemon for humans? Not only are they in the field egg group, which is mostly comprised of mammals, Vaporeon are an average of 3\"03' tall and 63.9 pounds, this means they're large enough to be able handle human dicks, and with their impressive Base Stats for HP and access to Acid Armor, you can be rough with one. Due to their mostly water based biology, there's no doubt in my mind that an aroused Vaporeon would be incredibly wet, so wet that you could easily have sex with one for hours without getting sore. They can also learn the moves Attract, Baby-Doll Eyes, Captivate, Charm, and Tail Whip, along with not having fur to hide nipples, so it'd be incredibly easy for one to get you in the mood. With their abilities Water Absorb and Hydration, they can easily recover from fatigue with enough water. No other Pokemon comes close to this level of compatibility. Also, fun fact, if you pull out enough, you can make your Vaporeon turn white. Vaporeon is literally built for human dick. Ungodly defense stat+high HP pool+Acid Armor means it can take cock all day, all shapes and sizes and still come for more");
    rect2->direction = TRD_TOP_MIDDLE;
    rect2->instructions = *instructions;
    rect2->component.anchor = rect->component.anchor;
    rect2->component.anchor.size.yType = VEC_RELATIVE;
    rect2->component.anchor.size.relY = 1.0;
    rect2->component.anchor.size.xType = VEC_RELATIVE;
    rect2->component.anchor.size.relX= 0.5;
    rect2->component.anchor.position.relX = 0.5;
    rect2->component.anchor.position.relY = 0;
    
    RectangleComponent *rect3 = newRectangle();
    alAppend(&g->components, &rect);
    rect3->component.anchor = rect->component.anchor;
    rect3->component.anchor.size.absY = 5;
    rect3->color = 3;
    rect3->component.anchor.origin.relY = 1;
    rect3->component.anchor.position.relX = 0.5;
    rect3->component.anchor.position.relY = 1;
    
    screenComponent = (Component*) g;
    alAppend(&g->components, &rect3);
    alAppend(&g->components, &rect2);
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
    move(0, 0);
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
}
