#pragma once
#include <string.h>
#include <stdbool.h>

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

extern const int MAX_USERNAME_LENGTH;
extern const int MIN_USERNAME_LENGTH;
bool characterAllowedInUsername(char testChar);

extern int connId;
extern char *username;
extern char *activeChannel;

int getConnId();
char *getUsername();
char *getActiveChannel();
ConnectionStatusType connectToServer();
UsernameSetResponseType trySetUsername(char *newUname);
ChannelJoinResponseType tryJoinChannel(char *channelName);