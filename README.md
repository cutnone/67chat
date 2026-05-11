# 67Chat

67Chat is a chat server and client which runs entirely in the terminal. It's complete with username selection, multiple channels, and text formatting.

## Usage
By default, the client will try to connect to the cloud-based server. If the server is offline, or you would rather connect to a locally hosted instance, run the client with the `-local` flag.

After executing the client, press space to connect. After connecting, you will be prompted to enter a username to chat under, followed by a channel to chat in. Text inputs added to and deleted from at any position by moving the cursor with the left and right arrow keys.

After that, you will be free to chat with anyone else connected to the same channel. Press ENTER to send messages.

Pressing ESCAPE will take you to the options menu. From there, you can change your username, change the channel, and exit the program.

There are a few message formatting options:
* `*sample*` will be *italic*.
* `^sample^` will be **bold** (on terminals which support it).
* `~sample~` will be <u>underlined</u>.

The client will exit when the connection is closed.

## UI
The interface is fully responsive (to a point; you will know when it's too small). Components will resize/reposition and text will wrap to accommodate most terminal sizes.

## Building From Source on WSL
### Server
`cd` into the `server` directory and run `make`.
The output file will be `main.out`.
### Client
`cd` into the `client` directory.
Make sure you have ncurses installed. If you don't, you can install it with `sudo apt-get install libncurses5-dev`.
Run `make`.
The output file will be `client.out`.