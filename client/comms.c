#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

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

const int MAX_USERNAME_LENGTH = 16;
const int MIN_USERNAME_LENGTH = 3;
bool characterAllowedInUsername(char testChar) {
    return (
        (testChar == '_') ||
        (testChar >= '0' && testChar <= '9') ||
        (testChar >= 'A' && testChar <= 'Z') ||
        (testChar >= 'a' && testChar <= 'z')
    );
    // We only allow Roman letters, numbers, and underscores.
}

int connId = 0;
char *username = NULL;
char *activeChannel = NULL;

int getConnId() {
    return connId;
}
char *getUsername() {
    if (username == NULL) return "";
    return username;
}
char *getActiveChannel() {
    if (activeChannel == NULL) return "";
    return activeChannel;
}

ConnectionStatusType connect() {
    connId = 5; // dummy test implementation
    return CONNECTION_OK;
}
UsernameSetResponseType trySetUsername(char *newUname) {
    
    // sanitize
    int len = strnlen(newUname, MAX_USERNAME_LENGTH+1);
    //                                             ^ +1 so we can tell the difference
    //                                             between > 16 and 16
    if (len < MIN_USERNAME_LENGTH) {
        return UNSET_TOO_SHORT;
    }
    if (len > MAX_USERNAME_LENGTH) {
        return UNSET_TOO_LONG;
    }
    for (int i = 0; i < len; i++) {
        char this = newUname[i];
        if (!characterAllowedInUsername(this)) {
            return UNSET_TOO_INVALID;
        }
    }
    if (len > MAX_USERNAME_LENGTH) {
        return UNSET_TOO_LONG;
    }

    free(username);
    username = newUname;
    return UNSET_OK;

}