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

typedef enum {
    PKT_SET_NAME = 1,
    PKT_JOIN,
    PKT_LEAVE,
    PKT_CHAT,
    PKT_LIST
} PacketType;

typedef struct PacketHeader { //for packets
    uint8_t type;      // which packet
    uint16_t length;   // payload size
} PacketHeader;

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

// client sending utility stuff
void send_to_client(int fd, char *msg){
    send(fd, msg, strlen(msg), 0);
}
void broadcast(int sender_fd, int channel_index, char *msg){
    for(int i=0; i<MAX_CLIENTS; i++){
        if (clients[i].fd != 0 && clients[i].state == STATE_CHANNEL && clients[i].channel_index == channel_index/* && clients[i].fd != sender_fd*/){
            send_to_client(clients[i].fd, msg);
        }
    }
}
void remove_client(int i){
    close(clients[i].fd);
    clients[i].fd = 0;
    clients[i].state = STATE_PENDING;
    clients[i].channel_index = -1;
    strcpy(clients[i].username, "ghost");
}
// channel utilities
int find_channel(char *name){
    for(int i=0; i<channel_count; i++){
        if(channels[i].active && strcmp(channels[i].name, name) == 0){
            return i;
        }
    }
    return -1;
}
void add_channel(char *name){
    if(channel_count >= MAX_CHANNELS) return;
    strcpy(channels[channel_count].name, name);
    channels[channel_count].active = 1;
    channel_count++;
}
//Commands
void handle_name(int i, char *name){
    if(clients[i].state == STATE_CHANNEL){
        send_to_client(clients[i].fd, "[SERVER] Leave the channel before trying to change your name!\n");
        return;
    }
    name[strcspn(name, "\r\n")] = 0; //trim the newlines
    strncpy(clients[i].username, name, MAX_USERNAME - 1);
    send_to_client(clients[i].fd, "[SERVER] Name Updated.\n");
}
void handle_join(int i, char *channel){
    if (clients[i].state == STATE_CHANNEL){
        send_to_client(clients[i].fd, "[SERVER] You're already in a channel!\n");
        return;
    }
    channel[strcspn(channel, "\r\n")] = 0; //trim the newlines
    int idx = find_channel(channel);
    if (idx == -1){
        send_to_client(clients[i].fd, "[SERVER] Channel not found!\n");
        return;
    }

    clients[i].state = STATE_CHANNEL;
    clients[i].channel_index = idx;

    send_to_client(clients[i].fd, "[SERVER] Joined channel successfully!\n");
}
void handle_leave(int i){
    if (clients[i].state != STATE_CHANNEL){
        send_to_client(clients[i].fd, "[SERVER] You can't leave a channel if you're not in a channel!\n");
        return;
    }
    clients[i].state = STATE_PENDING;
    clients[i].channel_index = -1;

    send_to_client(clients[i].fd, "[SERVER] Left channel successfully!\n");
}
void handle_list(int i){
    send_to_client(clients[i].fd, "[SERVER] Channels:\n");
    for(int j = 0; j < channel_count; j++){
        if (channels[j].active){
            char buf[BUFFER_SIZE];
            snprintf(buf, sizeof(buf), "- %s\n", channels[j].name);
            send_to_client(clients[i].fd, buf);
        }
    }
}
void handle_chat(int i, char *msg){
    if (clients[i].state != STATE_CHANNEL){
        send_to_client(clients[i].fd, "[SERVER] In order to chat, you must join a channel first!\n");
        return;
    }
    char out[BUFFER_SIZE];
    snprintf(out, sizeof(out), "[%s]: %s", clients[i].username, msg);
    broadcast(clients[i].fd, clients[i].channel_index, out);
}


int main(){
    int server_fd; //server socket file descriptor - id # of server socket; linux treats sockets as ints
    struct sockaddr_in server_addr; //make struct that stores where the server lives

    //initialize clients
    for(int i=0; i<MAX_CLIENTS; i++){
        clients[i].fd = 0;
        clients[i].state = STATE_PENDING;
        clients[i].channel_index = -1;
        strcpy(clients[i].username, "null");
    }

    //preset channels
    add_channel("channel 1");
    add_channel("channel 67");


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

    fd_set readfds; //set of scokets to monitor
    
    while(1){
        FD_ZERO(&readfds); //clear set
        FD_SET(server_fd, &readfds); //add server socket so we can detect new connections

        int max_fd = server_fd; //we want to know the highest numbered id so we can use select

        for(int i = 0; i<MAX_CLIENTS; i++){ //find highest numbered id while also getting a set of client sockets
            int fd = clients[i].fd;
            if (fd > 0) FD_SET(fd, &readfds); //add socket to watch set if client exists
            if (fd > max_fd) max_fd = fd;
        }

        // HOLD:
        select(max_fd + 1, &readfds, NULL, NULL, NULL);
        // code pauses here until something happens socket-wise



        // OK SOMETHING HAPPENED, NOW WHAT?:

        // new connection
        if (FD_ISSET(server_fd, &readfds)){ //check if server socket is "ready", which means a new client is connecting
            int new_fd = accept(server_fd, NULL, NULL); //accept the connecttion (returns the socket for the client)
            for(int i = 0; i < MAX_CLIENTS; i++){
                if (clients[i].fd == 0){ //find empty slot in my array
                    clients[i].fd = new_fd;
                    send_to_client(new_fd, "[SERVER] Welcome! Commands are:\n/name <n>\n/join <c>\n/leave\n/list\n");
                    break;
                }
            }
        }

        //handle existing clients
        for(int i = 0; i < MAX_CLIENTS; i++){
            int fd = clients[i].fd;
            if (fd>0 && FD_ISSET(fd, &readfds)){ //check if client exists & has sent
                PacketHeader header;
                int n = recv(fd, &header, sizeof(header), 0);

                if (n <= 0){
                    remove_client(i);
                    continue;
                }

                // read payload if it exists
                char payload[BUFFER_SIZE] = {0}; //make sure it's like ended properly to read
                if (header.length > 0){
                    recv(fd, payload, header.length, 0);
                }

                switch(header.type){
                    case PKT_SET_NAME: {
                        handle_name(i, payload);
                        break;
                    }
                    case PKT_JOIN: {
                        handle_join(i, payload);
                        break;
                    }
                    case PKT_LEAVE: {
                        handle_leave(i);
                        break;
                    }
                    case PKT_CHAT: {
                        handle_chat(i, payload);
                        break;
                    }
                    case PKT_LIST: {
                        handle_list(i);
                        break;
                    }
                    default: {
                        send_to_client(fd, "[SERVER] The server received an unknown packet type; this is a code problem, not your fault\n");
                        break;
                    }
                }
            }
        }

    } //keep alive loop;

    //ok techincally this can't be reached yet but ykyk just for good measure:
    close(server_fd);
    return 0;
}