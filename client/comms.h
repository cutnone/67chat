#pragma once
#include <string.h>
#include <stdbool.h>
#include "../common/datastructures/arrayList.h"

typedef enum {
    CONNECTION_OK,
    CONNECTION_FAILED,
} ConnectionStatusType;
typedef enum {
    UNSET_OK,
    UNSET_TOO_SHORT,
    UNSET_TOO_LONG,
    UNSET_INVALID,
    UNSET_TAKEN,
    UNSET_FAILED,
} UsernameSetResponseType;

typedef enum {
    CJOIN_OK,
    CJOIN_TOO_SHORT,
    CJOIN_TOO_LONG,
    CJOIN_INVALID,
    CJOIN_FAILED,
} ChannelJoinResponseType;

#ifdef _WIN32
    extern SOCKET sock;
#else
    extern int sock;
#endif
extern const int MAX_USERNAME_LENGTH;
extern const int MIN_USERNAME_LENGTH;
bool characterAllowedInUsername(char testChar);

extern bool connected;
extern char *username;
extern char *activeChannel;
extern char *reserveChannel;
extern ArrayList *messageCache;

int getSock();
char *getUsername();
char *getActiveChannel();
ConnectionStatusType connectToServer();
void disconnectFromServer();
UsernameSetResponseType trySetUsername(char *newUname);
ChannelJoinResponseType tryJoinChannel(char *channelName);
void leaveChannel();
void sendChatMessage(char *msg);
void receivePartialMessage();