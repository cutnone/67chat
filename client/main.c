#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "ui/scenes/connect.h"
#include "ui/scenes/testScene.h"
#include "ui/scenes/chooseUsername.h"
#include "ui/scenes/chatScene.h"
#include "ui/scenes/chooseChannel.h"
#include "ui/scenes/resizeScene.h"
#include "ui/scenes/optionsScene.h"

static void finish(int sig);
void handleResize(int _);

/*
    Much boilerplate code for ncurses has been borrowed 
    from an online tutorial:
    https://github.com/enthought/ncurses-5.5/blob/master/doc/ncurses-intro.doc
*/

int main() {
    signal(SIGINT, finish); 
    #ifndef _WIN32
        signal(SIGWINCH, resize);
    #endif
    initializeConnectScene();
    initializeTestScene();
    initializeChooseUsername();
    initializeChooseChannel();
    initializeChatScene();
    initializeResizeScene();
    initializeOptionsScene();
    screenComponent = (Component *) connectScene;
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