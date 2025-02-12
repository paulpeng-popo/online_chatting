#include <tcplib.h>
#include <sys/types.h>  // pid_t
#include <sys/socket.h> // socket, bind, listen, accept, setsockopt
#include <sys/wait.h>   // waitpid
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_ntoa
#include <unistd.h>     // close, fork, read, write
#include <string.h>     // memset, memcpy, strlen, strstr, strcmp, strcpy
#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <sys/mman.h>   // mmap, munmap

#define BUFSIZE 512
#define BACKLOG 5
#define MAXUSERS 5
#define RECORD 8
#define DEFAULT_PORT 12345
typedef struct UserNode
{
    char name[BUFSIZE];
    char ip[16];
    int userfd;
    int active;
    char messaage[RECORD][BUFSIZE];
    int top;
} UserNode;

static UserNode *shared_list = NULL;

int initServer()
{
    int port = DEFAULT_PORT;
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0)
    {
        printf("Socket creation failed.\n");
        exit(1);
    }

    struct sockaddr_in srv;
    memset(&srv, 0, sizeof(srv));

    srv.sin_family = PF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = htonl(INADDR_ANY);

    // reuse address and port
    setsockopt(
        fd,
        SOL_SOCKET,
        SO_REUSEADDR | SO_REUSEPORT,
        &(int){1},
        sizeof(int));

    if (bind(fd, (struct sockaddr *)&srv, sizeof(srv)) < 0)
    {
        printf("Binding failed.\n");
        exit(1);
    }

    if (listen(fd, BACKLOG) < 0)
    {
        printf("Listening failed.\n");
        exit(1);
    }

    // show server information
    printf("Server is listening on port %d\n", port);
    printf("Max number of users is %d\n", MAXUSERS);

    return fd;
}

void initSharedMemory()
{
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    size_t list_size = MAXUSERS * sizeof(UserNode);
    shared_list = mmap(NULL, list_size, prot, flags, -1, 0);

    // initialize shared memory
    for (int i = 0; i < MAXUSERS; i++)
    {
        memset(shared_list[i].name, 0, BUFSIZE);
        memset(shared_list[i].ip, 0, 16);
        shared_list[i].userfd = -1;
        shared_list[i].active = 0;
        memset(shared_list[i].messaage, 0, RECORD * BUFSIZE * sizeof(char));
        shared_list[i].top = 0;
    }
}

void closeSharedMemory()
{
    size_t list_size = MAXUSERS * sizeof(UserNode);
    munmap(shared_list, list_size);
}

void clientAcception(int fd)
{
    struct sockaddr_in cli;
    char cli_ip_addr[16];

    initSharedMemory();

    while (1)
    {
        memset(cli_ip_addr, 0, sizeof(cli_ip_addr));
        int new_fd = accept(
            fd,
            (struct sockaddr *)&cli,
            &(socklen_t){sizeof(cli)});
        if (new_fd < 0)
        {
            printf("Cannot accept client.\n");
            exit(1);
        }
        memcpy(cli_ip_addr, inet_ntoa(cli.sin_addr), 16);

        switch (fork())
        {
        case 0: // child process
            // close server socket
            close(fd);
            exit(clientRelay(new_fd, cli_ip_addr));
        default: // parent process
            // close client socket
            close(new_fd);
            break;
        case -1: // error
            printf("Fork failed.\n");
            exit(1);
        }
    }

    closeSharedMemory();
    return;
}

int clientRelay(int fd, char ip_addr[])
{
    char username[BUFSIZE];
    char buffer[BUFSIZE];
    size_t size = 0;
    int index = -1;

    while (1)
    {
        memset(username, 0, BUFSIZE);
        memset(buffer, 0, BUFSIZE);

        read(fd, username, BUFSIZE);

        index = online(fd, username, ip_addr);
        if (index != -1)
        {
            write(fd, "SUC", BUFSIZE);
            size = strlen(username) + 27;
            snprintf(buffer, size, "User %s[%s]%s login\n", CYAN, username, RESET);
            bcast(username, buffer);
            printf("%s", buffer);
            break;
        }
        else
            write(fd, "DUP", BUFSIZE);
    }

    char private_target[BUFSIZE];
    char receive[BUFSIZE];
    pid_t cpid = 1;

    while (1)
    {
        memset(private_target, 0, BUFSIZE);
        memset(receive, 0, BUFSIZE);
        memset(buffer, 0, BUFSIZE);

        if (cpid)
            cpid = fork();
        else
            cpid = 1;

        switch (cpid)
        {
        case 0:
            read(fd, receive, BUFSIZE);

            if (!strcmp(receive, "bye") || !strlen(receive))
            {
                if (!offline(index))
                {
                    // printf("System error\n");
                    exit(1);
                }
                else
                {
                    size = strlen(username) + 28;
                    snprintf(buffer, size, "User %s[%s]%s offline\n", CYAN, username, RESET);
                    bcast(username, buffer);
                    printf("%s", buffer);
                }
                break;
            }

            // extract user from unicast message
            char *pch = strstr(receive, "\\>");
            if (pch != NULL)
            {
                char *temp = pch;
                pch += 2;
                while (*pch == ' ')
                    pch++;
                strcpy(private_target, pch);
                memset(temp, 0, strlen(private_target) + 2);

                if (strcmp(private_target, username))
                {
                    ucast(index, private_target, receive);
                    printf(">> %s to %s: %s\n", username, private_target, receive);
                }
            }
            else
            {
                if (!strcmp(receive, "list"))
                    statusCmd(fd);
                else
                {
                    memset(buffer, 0, BUFSIZE);
                    size = strlen(receive) + strlen(username) + 19;
                    snprintf(buffer, size, ">> %s%s%s: %s\n", CYAN, username, RESET, receive);
                    bcast(username, buffer);
                    printf("%s", buffer);
                }
            }
            exit(1);
        default:
            sleep(1);
            cpid = waitpid(-1, NULL, WNOHANG);
            // unify all information by signal all clients print out buffer
            synch(fd, index);
            break;
        case -1:
            printf("Error occurs when forking processes.\n");
            exit(1);
        }
    }
    return 1;
}

int online(int fd, char username[], char ip_addr[])
{
    int index = -1;
    for (int i = 0; i < MAXUSERS; i++)
    {
        // if user login before
        if (shared_list[i].userfd != -1 && !strcmp(shared_list[i].name, username))
        {
            // user login again
            if (shared_list[i].active == 0)
            {
                memcpy(shared_list[i].ip, ip_addr, 16);
                shared_list[i].userfd = fd;
                shared_list[i].active = 1;
                index = i;
                break;
            }
            // duplicate login
            else
                return -1;
        }
    }

    // if user login first time
    if (index == -1)
    {
        for (int i = 0; i < MAXUSERS; i++)
        {
            // find empty slot
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

int offline(int index)
{
    if (shared_list[index].active == 1)
    {
        shared_list[index].active = 0;
        return 1;
    }
    return 0;
}

void synch(int fd, int index)
{
    for (int i = 0; i < shared_list[index].top; i++)
        write(fd, (shared_list[index].messaage)[i], BUFSIZE);
    memset(shared_list[index].messaage, 0, RECORD * BUFSIZE * sizeof(char));
    shared_list[index].top = 0;
    write(fd, "END", BUFSIZE);
}

void bcast(char username[], char notif[])
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

void ucast(int index, char target[], char msg[])
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
        else
            return;
    }
    else
        sprintf(note, "%s<User %s is not exist>%s\n", RED, target, RESET);

    int top = shared_list[index].top;
    memcpy((shared_list[index].messaage)[top], note, BUFSIZE);
    shared_list[index].top += 1;
}

void statusCmd(int fd)
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
