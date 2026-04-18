#include <signal.h>
#include <stdlib.h>
#include "interface.h"

static void finish(int sig);
void handleResize(int _);

/*
    Much boilerplate code for ncurses has been borrowed 
    from an online tutorial:
    https://github.com/enthought/ncurses-5.5/blob/master/doc/ncurses-intro.doc
*/

int main() {
    signal(SIGINT, finish); 
    signal(SIGWINCH, resize);
    init();
    
    rerender();
    for (;;) {
        checkKeystrokes();
    }

    // finish(0);
}

static void finish(int sig) {
    cleanup();
    exit(0);
}
void handleResize(int _) {
    resize();
}