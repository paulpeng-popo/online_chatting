# On-line Chatting System

## Introduction
Use multi-processes skill implement server side and client side program in broadcast and unicast functions. `switch(fork)` and `waitpid(-1, NULL, WNOHANG)` were used to handle processes, furthermore, I try a conditional forking skill listening background to receive broadcast message. The last but not the least, a shared memory skill is also introduced for server processes communicating.

## Dependencies

- CMake

## Build
Use CMake to build both the client and server program

```bash
mkdir build && cd build
cmake ..
make
```
Then you'll see the client and server program under the same directory

## Usage
### Client Usage

- Use `./client <server port num>` to start the program

#### GUI Usage

- Login with
    1. Type a login username
    2. If a new username detected, system create new account
    3. if you had login before, re-use the account and check offline message
- Chat command (examples)
    - Help: Type `help` to show all commands
    - Send: Type anything to send broadcast message
        ```bash
        Hello I am paulpeng
        ```
    - Send to user: `"message" \> <user>`, Type `\>` after any message where concatenate with a `<user>` name
        ```bash
        You are so pretty \> marie rose
        ```
    - List: Type `list` to list out all on-line and off-line users
    - Exit: Type `bye` to exit the program.

### Server Usage

- Use `./server <port number>` to hang over the program
- Debug: see all typing messages in the console
