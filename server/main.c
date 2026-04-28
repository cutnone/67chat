#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>

// constants:
#define PORT 6767
#define MAX_CLIENTS 67
#define BUFFER_SIZE 1024
#define MAX_USERNAME 24
#define MAX_CHANNEL_NAME 24
#define MAX_CHANNELS 67

typedef enum {
    STATE_PENDING,
    STATE_CHANNEL
} ClientState;

typedef struct Channel { //boom i can now use it either way
    char name[MAX_CHANNEL_NAME];
    int active;
} Channel;

typedef struct Client {
    int fd;
    char username[MAX_USERNAME];
    ClientState state;
    int channel_index; // gonna just do a "-1 if none" type situation
} Client;

Client clients[MAX_CLIENTS]; 
Channel channels[MAX_CHANNELS];
int channel_count = 0;

int main(){
    int server_fd; //server socket file descriptor - id # of server socket; linux treats sockets as ints
    struct sockaddr_in server_addr; //make struct that stores where the server lives
    server_fd = socket(AF_INET/*for ipv4*/, SOCK_STREAM/*for TCP*/, 0/*default protocol*/); //create the actual socket
    if (server_fd < 0){
        perror("socket creation failed; exiting");
        return 67;
    }
    
    //configure the server...
    server_addr.sin_family = AF_INET; //ipv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //accept from anything; localhost, wifi, ethernet, etc
    server_addr.sin_port = htons(PORT); //convert port number safely 

    //now we gotta actually bind it
    int bind_return_value = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)); //claim the port
    if (bind_return_value < 0){
        perror("socket binding to port failed; exiting");
        close(server_fd); //ensure we clean it up!!!
        return 67;
    }

    int listen_return_value = listen(server_fd, 10); //socket can now wait for clients; 10 is backlog queue size btw
    if (listen_return_value < 0){
        perror("listening failed; exiting");
        close(server_fd);
        return 67;
    }

    printf("The server is successfully listening on port %d\n", PORT);
    
    while(1){sleep(1);} //keep alive loop; while 1 is temp

    //ok techincally this can't be reached yet but ykyk just for good measure:
    close(server_fd);
    return 0;
}