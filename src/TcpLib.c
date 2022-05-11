#include <TcpLib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

extern int errno;

int ServerInit(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));

    srv.sin_family = PF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(fd, (struct sockaddr*) &srv, sizeof(srv)) < 0) {
        printf("Error occurs when binding.\n");
        return (-1);
    }

    if (listen(fd, BACKLOG) < 0) {
        printf("Error occurs when listening.\n");
        return (-1);
    }

    // printf("Server listening on port: %d\n\n", port);

    return fd;
}

void MultiProc(int fd)
{
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    size_t list_size = MAXUSERS*sizeof(UserNode);
    static UserNode* shared_list = NULL;
    shared_list = mmap(NULL, list_size, prot, flags, -1, 0);
    nodeClear(shared_list);

    char cli_ip_addr[16];
    // signal(SIGCHLD, reaper);

    while(1) {
        memset(cli_ip_addr, 0, sizeof(cli_ip_addr));
        int sfd = ServerWork(fd, cli_ip_addr);
        if (sfd < 0)
        {
            // if (errno == EINTR) continue;
            printf("Error occurs when accepting client.\n");
            exit(1);
        }

        switch (fork()) {
            case 0:
                close(fd);
                exit(RelayCLI(sfd, shared_list, cli_ip_addr));
            default:
                close(sfd);
                break;
            case -1:
                printf("Error occurs when forking processes.\n");
                exit(1);
        }
    }
    // munmap(shared_list, list_size);
}

int ServerWork(int fd, char ip_addr[])
{
    int new_fd;
    struct sockaddr_in cli;
    int cli_len = sizeof(cli);

    if ((new_fd = accept(fd, (struct sockaddr*) &cli, &cli_len)) < 0) {
        return (-1);
    }

    // map ip address to username in shared memory
    memcpy(ip_addr, inet_ntoa(cli.sin_addr), sizeof(inet_ntoa(cli.sin_addr))+1);
    return new_fd;
}

int ClientInit(char ip_addr[], int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));

    srv.sin_family = PF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = inet_addr(ip_addr);

    if (connect(fd, (struct sockaddr*) &srv, sizeof(srv)) < 0)
    {
        printf("Error occurs when connecting to server.\n");
        exit(1);
    }

    return fd;
}

void Chatting(int fd)
{
    char reply[BUFSIZE];
    char username[BUFSIZE];
    int ch = 0;

    system("clear");
    welcome();

    while(1)
    {
        // login user
        memset(reply, 0, BUFSIZE);
        memset(username, 0, BUFSIZE);

        printf("Login username: ");
        for (int i = 0; i < BUFSIZE; i++)
        {
            if ((ch = getc(stdin)) != EOF)
            {
                if (ch == 10)
                {
                    username[i] = '\0';
                    break;
                }
                username[i] = ch;
            }
        }
        write(fd, username, BUFSIZE);
        read(fd, reply, BUFSIZE);

        if (!memcmp(reply, "DUP", 3))
            printf("%sUsername %s%s%s is taken by other user%s\n", RED, CYAN, username, RED, RESET);
        if (!memcmp(reply, "SUC", 3))
            break;
    }

    system("clear");
    printf("User %s[%s]%s login successfully\n\n", CYAN, username, RESET);

    // fflush stdin
    // scanf("%*[^\n]");
    // scanf("%*c");

    // shared memory
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    static char* send = NULL;
    send = mmap(NULL, BUFSIZE, prot, flags, -1, 0);
    pid_t cpid = 1;

    while(1)
    {
        memset(reply, 0, BUFSIZE);

        // conditional forking skill
        if(cpid) cpid = fork();
        else cpid = 1;

        switch (cpid) {
            case 0:
                while(1) {
                    memset(send, 0, BUFSIZE);
                    for (int i = 0; i < BUFSIZE; i++)
                    {
                        // scanf command
                        if ((ch = getc(stdin)) != EOF)
                        {
                            if (ch == 10)
                            {
                                send[i] = '\0';
                                break;
                            }
                            send[i] = ch;
                        }
                    }
                    if (!strcmp(send, "help")) help();
                    else break;
                };
                write(fd, send, BUFSIZE);
                exit(1);
            default:
                // non-blocking wait
                cpid = waitpid(-1, NULL, WNOHANG);
                while(1)
                {
                    memset(reply, 0, BUFSIZE);
                    read(fd, reply, BUFSIZE);
                    if (!memcmp(reply, "END", 3)) break;
                    printf("%s", reply);
                }
                break;
            case -1:
                printf("Error occurs when forking processes.\n");
                exit(1);
        }

        if (!strcmp(send, "bye")) break;
    }
    munmap(send, BUFSIZE);
}

int RelayCLI(int fd, UserNode* shared_list, char ip_addr[])
{
    char username[BUFSIZE];
    char buffer[BUFSIZE];
    size_t size = 0;
    int index = -1;

    while(1)
    {
        memset(username, 0, BUFSIZE);
        memset(buffer, 0, BUFSIZE);

        read(fd, username, BUFSIZE);

        // record username location in shared memory
        index = online(fd, shared_list, username, ip_addr);
        if (index != -1)
        {
            write(fd, "SUC", BUFSIZE);
            size = strlen(username) + 27;
            snprintf(buffer, size, "User %s[%s]%s login\n", CYAN, username, RESET);
            bcast(shared_list, username, buffer);
            printf("%s", buffer);
            break;
        }
        else write(fd, "DUP", BUFSIZE);
    }

    char private_target[BUFSIZE];
    char receive[BUFSIZE];
    pid_t cpid = 1;

    while(1)
    {
        memset(private_target, 0, BUFSIZE);
        memset(receive, 0, BUFSIZE);
        memset(buffer, 0, BUFSIZE);

        if(cpid) cpid = fork();
        else cpid = 1;

        switch (cpid) {
            case 0:
                read(fd, receive, BUFSIZE);

                if (!strcmp(receive, "bye") || !strlen(receive))
                {
                    if (!offline(shared_list, index))
                    {
                        // printf("System error\n");
                        exit(1);
                    }
                    else
                    {
                        size = strlen(username) + 28;
                        snprintf(buffer, size, "User %s[%s]%s offline\n", CYAN, username, RESET);
                        bcast(shared_list, username, buffer);
                        printf("%s", buffer);
                    }
                    break;
                }

                // extract user from unicast message
                char* pch = strstr(receive, "\\>");
                if (pch != NULL)
                {
                    char* temp = pch;
                    pch += 2;
                    while(*pch == ' ') pch++;
                    strcpy(private_target, pch);
                    memset(temp, 0, strlen(private_target)+2);

                    if (strcmp(private_target, username))
                    {
                        ucast(shared_list, index, private_target, receive);
                        printf(">> %s to %s: %s\n", username, private_target, receive);
                    }
                }
                else
                {
                    if (!strcmp(receive, "list"))
                        statusCmd(fd, shared_list);
                    else
                    {
                        memset(buffer, 0, BUFSIZE);
                        size = strlen(receive) + strlen(username) + 19;
                        snprintf(buffer, size, ">> %s%s%s: %s\n", CYAN, username, RESET, receive);
                        bcast(shared_list, username, buffer);
                        printf("%s", buffer);
                    }
                }
                exit(1);
            default:
                sleep(1);
                cpid = waitpid(-1, NULL, WNOHANG);
                // unify all information by signal all clients print out buffer
                synch(fd, shared_list, index);
                break;
            case -1:
                printf("Error occurs when forking processes.\n");
                exit(1);
        }
    }
    return 1;
}

// void reaper(int sig)
// {
//     int status;
//     while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
// }

void nodeClear(UserNode* shared_list)
{
    for (int i = 0; i < MAXUSERS; i++)
    {
        memset(shared_list[i].name, 0, BUFSIZE);
        memset(shared_list[i].ip, 0, 16);
        shared_list[i].userfd = -1;
        shared_list[i].active = 0;
        memset(shared_list[i].messaage, 0, RECORD*BUFSIZE*sizeof(char));
        shared_list[i].top = 0;
    }
}

int online(int fd, UserNode* shared_list, char username[], char ip_addr[])
{
    int index = -1;
    for (int i = 0; i < MAXUSERS; i++)
    {
        if (shared_list[i].userfd != -1 && !strcmp(shared_list[i].name, username))
        {
            if (shared_list[i].active == 0)
            {
                memcpy(shared_list[i].ip, ip_addr, 16);
                shared_list[i].userfd = fd;
                shared_list[i].active = 1;
                index = i;
                break;
            }
            else return -1;
        }
    }

    if (index == -1)
    {
        for (int i = 0; i < MAXUSERS; i++)
        {
            if (shared_list[i].userfd == -1)
            {
                memcpy(shared_list[i].name, username, BUFSIZE);
                memcpy(shared_list[i].ip, ip_addr, 16);
                shared_list[i].userfd = fd;
                shared_list[i].active = 1;
                index = i;
                break;
            }
        }
    }

    return index;
}

int offline(UserNode* shared_list, int index)
{
    if (shared_list[index].active == 1)
    {
        shared_list[index].active = 0;
        return 1;
    }
    return 0;
}

void synch(int fd, UserNode* shared_list, int index)
{
    for (int i = 0; i < shared_list[index].top; i++)
        write(fd, (shared_list[index].messaage)[i], BUFSIZE);
    memset(shared_list[index].messaage, 0, RECORD*BUFSIZE*sizeof(char));
    shared_list[index].top = 0;
    write(fd, "END", BUFSIZE);
}

void bcast(UserNode* shared_list, char username[], char notif[])
{
    for (int i = 0; i < MAXUSERS; i++)
    {
        if (shared_list[i].active == 1 && strcmp(shared_list[i].name, username))
        {
            int top = shared_list[i].top;
            memcpy((shared_list[i].messaage)[top], notif, BUFSIZE);
            shared_list[i].top += 1;
        }
    }
}

void ucast(UserNode* shared_list, int index, char target[], char msg[])
{
    int target_index = -1;
    char note[BUFSIZE];
    memset(note, 0, BUFSIZE);
    size_t size = strlen(shared_list[index].name) + strlen(msg) + 43;

    for (int i = 0; i < MAXUSERS; i++)
    {
        if (shared_list[i].userfd != -1 && !strcmp(shared_list[i].name, target))
        {
            target_index = i;
            break;
        }
    }

    if (target_index != -1)
    {
        snprintf(note, size, ">> %s%s%s [whisper to you]%s: %s\n", CYAN, shared_list[index].name, YELLOW, RESET, msg);
        int top = shared_list[target_index].top;
        memcpy((shared_list[target_index].messaage)[top], note, BUFSIZE);
        shared_list[target_index].top += 1;
        memset(note, 0, BUFSIZE);

        if (shared_list[target_index].active == 0)
            sprintf(note, "%s<User %s is offline, your message will queue until the user is online>%s\n", RED, target, RESET);
        else return;
    }
    else sprintf(note, "%s<User %s is not exist>%s\n", RED, target, RESET);

    int top = shared_list[index].top;
    memcpy((shared_list[index].messaage)[top], note, BUFSIZE);
    shared_list[index].top += 1;
}

void statusCmd(int fd, UserNode* shared_list)
{
    char temp[10];
    char info[BUFSIZE];

    for (int i = 0; i < MAXUSERS; i++)
    {
        memset(info, 0, BUFSIZE);
        memset(temp, 0, sizeof(temp));

        if (shared_list[i].userfd != -1)
        {
            if (shared_list[i].active)
            {
                memcpy(temp, "[online]", 8);
                size_t size = strlen(shared_list[i].name) + strlen(temp) + 31;
                snprintf(info, size, "%s%s%s User: %s%s%s\n", GREEN, temp, RESET, CYAN, shared_list[i].name, RESET);
            }
            else
            {
                memcpy(temp, "[offline]", 9);
                size_t size = strlen(shared_list[i].name) + strlen(temp) + 31;
                snprintf(info, size, "%s%s%s User: %s%s%s\n", RED, temp, RESET, CYAN, shared_list[i].name, RESET);
            }
            write(fd, info, BUFSIZE);
        }
    }
    write(fd, "END", BUFSIZE);
}
