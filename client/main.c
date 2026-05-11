#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    // windows support taught by claude
#else  
    #include <sys/select.h>
    #include <unistd.h>
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "comms.h"
#include "ui/scenes/connect.h"
#include "ui/scenes/chooseUsername.h"
#include "ui/scenes/chatScene.h"
#include "ui/scenes/chooseChannel.h"
#include "ui/scenes/resizeScene.h"
#include "ui/scenes/optionsScene.h"

static void finish(int sig);
void handleResize(int _);

int main(int argc, char *argv[]) {

    // use local server if specified, otherwise use the cloud server
    if (argc >= 2 && strcmp(argv[1], "-local") == 0) {
        serverAddress = "127.0.0.1";
    }

    signal(SIGINT, finish); 
    #ifdef _WIN32
        HANDLE stdInHandle = GetStdHandle(STD_INPUT_HANDLE);
        WSAEVENT sockEvent = WSACreateEvent();
        HANDLE handles[2] = {stdInHandle, sockEvent};
    #else
        signal(SIGWINCH, resize);
    #endif

    // initialize all the scenes!
    initializeConnectScene();
    initializeChooseUsername();
    initializeChooseChannel();
    initializeChatScene();
    initializeResizeScene();
    initializeOptionsScene();

    // initialize UI
    screenComponent = (Component *) connectScene;
    init();
    rerender();

    // network/input loop
    while (true) {
        #ifdef _WIN32
            if (connected) {
                WSAEventSelect(sock, sockEvent, FD_READ | FD_CLOSE);
            }
            DWORD ready = WaitForMultipleObjects(connected ? 2 : 1, handles, FALSE, 100);
            if (ready == WAIT_OBJECT_0) {
                // stdin
                checkKeystrokes();
            } else if (ready == WAIT_OBJECT_0+1) {
                receivePartialMessage();
                rerender();
            }
        #else
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
        #endif
    }

}

static void finish(int sig) {
    cleanup();
    exit(0);
}
void handleResize(int _) {
    resize();
}