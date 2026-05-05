#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "comms.h"
#include "ui/scenes/connect.h"
#include "ui/scenes/testScene.h"
#include "ui/scenes/chooseUsername.h"
#include "ui/scenes/chatScene.h"
#include "ui/scenes/chooseChannel.h"
#include "ui/scenes/resizeScene.h"
#include "ui/scenes/optionsScene.h"
#ifdef _WIN32
#else  
    #include <sys/select.h>
    #include <unistd.h>
#endif

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
    while (true) {
        fd_set fds;
        FD_ZERO(&fds);
        int maxfd = STDIN_FILENO + 1;
        if (connected) {
            maxfd = (sock > STDIN_FILENO ? sock : STDIN_FILENO) + 1;
            FD_SET(sock, &fds);
        }
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {.tv_sec = 0, .tv_usec = 100000};
        select(maxfd, &fds, NULL, NULL, &tv);
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            checkKeystrokes();
        }
        if (FD_ISSET(sock, &fds)) {
            receivePartialMessage();
            rerender();
        }
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