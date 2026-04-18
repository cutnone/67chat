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
    UNSET_TOO_INVALID,
    UNSET_TAKEN,
    UNSET_FAILED,
} UsernameSetResponseType;

const int MAX_USERNAME_LENGTH;
const int MIN_USERNAME_LENGTH;
bool characterAllowedInUsername(char testChar);

int connId;
char *username;
char *activeChannel;

int getConnId();
char *getUsername();
char *getActiveChannel();
ConnectionStatusType connect();
UsernameSetResponseType trySetUsername(char *newUname);