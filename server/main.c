#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>

#include "../common/datastructures/arrayList.h"

//server is <500 lines so idk if it's even worth separating; it's actually quite nice and clean like this unlike the complicated client (oh god)

// constants: (must match client)
#define PORT 6767
#define BUFFER_SIZE 1024
#define MAX_USERNAME 16
#define MAX_CHANNEL_NAME 16
#define MIN_USERNAME 3
#define MIN_CHANNEL_NAME 3

// !!! MUST MATCH comms.c !!!
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
// end stuff from comms.c lol

//helper from client ig 
int character_allowed(char c) {
    return (
        c == '_' ||
        (c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z')
    );
}

typedef struct Channel { //boom i can now use it either way
    char name[MAX_CHANNEL_NAME];
    int is_default;
} Channel;

typedef struct Client {
    int fd;
    char username[MAX_USERNAME];
    ClientState state;
    int channel_index; // gonna just do a "-1 if none" type situation
} Client;

ArrayList *clients;
ArrayList *channels;

void try_delete_channel(int channel_index);

// client sending utility stuff
void send_to_client(int fd, char *msg){
    printf("msg: %s\n", msg);
    send(fd, msg, strlen(msg)+1 /* include null byte */, 0);
}
void broadcast(int sender_fd, int channel_index, char *msg){
    for (int i = 0; i < clients->length; i++){
        Client *c = (Client *)alGet(clients, i);
        if (!c) continue;
        if (c->state == STATE_CHANNEL && c->channel_index == channel_index){
            send_to_client(c->fd, msg);
        }
    }
}
void remove_client(int i){
    Client *c = (Client *)alGet(clients, i);
    if (!c) return;
    int old_channel = c->channel_index;
    close(c->fd);
    alRemove(clients, i); //shifts everything left
    if (old_channel != -1){
        try_delete_channel(old_channel);
    }
}
// channel utilities
int find_channel(char *name){
    for (int i = 0; i < channels->length; i++){
        Channel *ch = (Channel *)alGet(channels, i);
        if (ch && strcmp(ch->name, name) == 0){
            return i;
        }
    }
    return -1;
}
int add_channel(char *name, int is_default){
    Channel ch;
    strncpy(ch.name, name, MAX_CHANNEL_NAME-1);
    ch.name[MAX_CHANNEL_NAME-1] = '\0';
    ch.is_default = is_default;
    alAppend(channels, &ch);
    return channels->length-1;
}
void remove_channel(int index){
    if (index < 0 || index >= (int)channels->length) return; //might wanna intcast it just in case so yea
    alRemove(channels, index);
    // now we gotta fix client indices
    for (int i = 0; i < clients->length; i++){
        Client *c = (Client *)alGet(clients, i);
        if (!c) continue;
        if (c->channel_index == index){
            c->channel_index = -1;
            c->state = STATE_PENDING;
            send_to_client(c->fd, "[SERVER] The channel you were in was deleted.\n"); //shouldn't be possible unless there's some strange race condition
        } else if (c->channel_index > index){
            c->channel_index--;
        }
    }
}
void try_delete_channel(int channel_index){ //function to check if a channel should be deleted ig lol
    if (channel_index < 0 || channel_index >= (int)channels->length) return;

    Channel *ch = (Channel *)alGet(channels, channel_index);
    if (!ch || ch->is_default) return; // if it's default channel or doesnt even exist then stop

    // check if theres any clients still connected to this channel
    for (int i = 0; i < clients->length; i++){
        Client *c = (Client *)alGet(clients, i);
        if (!c) continue;
        if (c->state == STATE_CHANNEL && c->channel_index == channel_index){
            return;
        }
    }

    remove_channel(channel_index);
}
//utility to sanitize stuff
int validate_username(char *name) {
    int len = strnlen(name, MAX_USERNAME + 1);

    if (len < MIN_USERNAME) return 0;
    if (len > MAX_USERNAME) return 0;

    for (int i = 0; i < len; i++) {
        if (!character_allowed(name[i])) return 0;
    }

    if (strcmp(name, "SERVER") == 0) return 0;

    return 1;
}
int validate_channel(char *name) {
    int len = strnlen(name, MAX_CHANNEL_NAME + 1);

    if (len < MIN_CHANNEL_NAME) return 0;
    if (len > MAX_CHANNEL_NAME) return 0;

    for (int i = 0; i < len; i++) {
        if (!character_allowed(name[i])) return 0;
    }

    return 1;
}
int validate_message(char *msg) {
    int len = strnlen(msg, BUFFER_SIZE);

    if (len == 0) return 0;
    if (len >= BUFFER_SIZE - 1) return 0;

    return 1;
}

//Commands
void handle_name(int i, char *name){
    Client *c = (Client *)alGet(clients, i);
    if (!c) return;

    if(c->state == STATE_CHANNEL){
        send_to_client(c->fd, "[SERVER] Leave the channel before trying to change your name!");
        return;
    }
    name[strcspn(name, "\r\n")] = 0; //trim at newline/end

    if (!validate_username(name)) {
        send_to_client(c->fd, "[SERVER] Invalid username!\n");
        return;
    }

    strncpy(c->username, name, MAX_USERNAME - 1);
    c->username[MAX_USERNAME - 1] = '\0';
    
    send_to_client(c->fd, "[SERVER] Name Updated.");
}
void handle_join(int i, char *channel){
    Client *c = (Client *)alGet(clients, i);
    if (!c) return;
    if (c->state == STATE_CHANNEL){
        send_to_client(c->fd, "[SERVER] You're already in a channel!");
        return;
    }
    channel[strcspn(channel, "\r\n")] = 0;
    if (!validate_channel(channel)) {
        send_to_client(c->fd, "[SERVER] Invalid channel name!");
        return;
    }
    int idx = find_channel(channel);
    if (idx == -1){
        idx = add_channel(channel, 0); // auto-create if doesn't exist
        send_to_client(c->fd, "[SERVER] Created new channel!");
    }
    c->state = STATE_CHANNEL;
    c->channel_index = idx;
    send_to_client(c->fd, "[SERVER] Joined channel successfully!");
}
void handle_leave(int i){
    Client *c = (Client *)alGet(clients, i);
    if (!c) return;

    if (c->state != STATE_CHANNEL){
        send_to_client(c->fd, "[SERVER] You can't leave a channel if you're not in a channel!");
        return;
    }
    
    int old_channel = c->channel_index;
    c->state = STATE_PENDING;
    c->channel_index = -1;
    try_delete_channel(old_channel);

    send_to_client(c->fd, "[SERVER] Left channel successfully!");
}
void handle_list(int i){
    Client *c = (Client *)alGet(clients, i);
    if (!c) return;
    
    send_to_client(c->fd, "[SERVER] Active channels:\n");
    for (int j = 0; j < channels->length; j++){
        Channel *ch = (Channel *)alGet(channels, j);
        if (ch){
            char buf[BUFFER_SIZE];
            snprintf(buf, sizeof(buf), "- %s\n", ch->name);
            send_to_client(c->fd, buf);
        }
    }
}
void handle_chat(int i, char *msg){
    Client *c = (Client *)alGet(clients, i);
    if (!c) return;

    if (c->state != STATE_CHANNEL){
        send_to_client(c->fd, "[SERVER] In order to chat, you must join a channel first!");
        return;
    }
    if (!validate_message(msg)) {
        send_to_client(c->fd, "[SERVER] Invalid message!\n");
        return;
    }
    char out[BUFFER_SIZE];
    snprintf(out, sizeof(out), "[%s]: %s", c->username, msg);
    broadcast(c->fd, c->channel_index, out);
}

int main(){
    channels = newArrayList(sizeof(Channel));
    clients = newArrayList(sizeof(Client));
    
    int server_fd; //server socket file descriptor - id # of server socket; linux treats sockets as ints
    struct sockaddr_in server_addr; //make struct that stores where the server lives

    //preset channels
    add_channel("channel1", 1); //1 means default
    add_channel("channel67", 1); //1 means default


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

        for (int i = 0; i < clients->length; i++){ //find highest numbered id while also getting a set of client sockets
            Client *c = (Client *)alGet(clients, i);
            if (!c) continue;
            int fd = c->fd;
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
            Client new_client;
            new_client.fd = new_fd;
            new_client.state = STATE_PENDING;
            new_client.channel_index = -1;
            strcpy(new_client.username, "Nameless");
            alAppend(clients, &new_client);
            send_to_client(new_fd, "[SERVER] Welcome! Commands are:\n/name <name>\n/join <channel>\n/leave\n/list\n");
        }

        //handle existing clients
        for(int i = clients->length - 1; i >= 0; i--){ //!!! WE HAVE TO ITERATE BACKWARDS BECAUSE REMOVING CLIENTS SHIFTS BACKWARDS
            Client *c = (Client *)alGet(clients, i);
            if (!c) continue;
            int fd = c->fd;

            if (fd > 0 && FD_ISSET(fd, &readfds)){
                PacketHeader header;
                int n = recv(fd, &header, sizeof(header), 0);

                if (n <= 0){
                    remove_client(i);
                    continue;
                }
                if (n != sizeof(header)) {
                    //smth weird happened; possible in tcp but let's just not
                    remove_client(i);
                    continue;
                }

                char payload[BUFFER_SIZE] = {0};
                if (header.length > 0){
                    if (header.length >= BUFFER_SIZE){
                        remove_client(i);
                        continue;
                    }
                    recv(fd, payload, header.length, 0);
                    payload[header.length] = '\0';
                }

                switch(header.type){ //i made this part cleaner :)
                    case PKT_SET_NAME:  handle_name(i, payload);   break;
                    case PKT_JOIN:      handle_join(i, payload);   break;
                    case PKT_LEAVE:     handle_leave(i);           break;
                    case PKT_CHAT:      handle_chat(i, payload);   break;
                    case PKT_LIST:      handle_list(i);            break;
                    default: send_to_client(fd, "[SERVER] Unknown packet type\n");
                }
            }
        }

    } //keep alive loop;

    //ok techincally this can't be reached but just for good measure / code practice:
    close(server_fd);
    alFree(channels);
    alFree(clients);
    return 0;
}