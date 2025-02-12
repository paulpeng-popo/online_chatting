#include <clienthelp.h>
#include <stdio.h>      // printf
#include <sys/socket.h> // socket
#include <unistd.h>     // read, write
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr
#include <string.h>     // memset
#include <stdlib.h>     // exit
#include <sys/mman.h>   // mmap, munmap
#include <sys/wait.h>   // waitpid
#include <sys/types.h>  // pid_t
#include <time.h>       // time, localtime

#define BUFSIZE 512

int initClient(char ip_addr[], int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in cli;
    memset(&cli, 0, sizeof(cli));

    cli.sin_family = PF_INET;
    cli.sin_port = htons(port);
    cli.sin_addr.s_addr = inet_addr(ip_addr);

    if (connect(fd, (struct sockaddr *)&cli, sizeof(cli)) < 0)
    {
        printf("Error occurs when connecting to server.\n");
        exit(1);
    }

    return fd;
}

void startChat(int fd)
{
    char reply[BUFSIZE];
    char username[BUFSIZE];
    char inputc = 0;

    while (1)
    {
        // login user
        memset(reply, 0, BUFSIZE);
        memset(username, 0, BUFSIZE);

        printf("Login username: (a-z, A-Z, 0-9, -, _)\n");
        for (int i = 0; i < BUFSIZE; i++)
        {
            if ((inputc = getc(stdin)) != EOF)
            {
                if (inputc == 10)
                {
                    username[i] = '\0';
                    break;
                }
                username[i] = inputc;
            }
            if (i == BUFSIZE - 1)
            {
                printf("Username is too long.\n");
                username[BUFSIZE - 1] = '\0';
                printf("Truncate to %s\n", username);
            }
        }

        printf("Your username is %s\n", username);
        write(fd, username, BUFSIZE);
        read(fd, reply, BUFSIZE);

        if (!memcmp(reply, "DUP", 3))
            printf(
                "%sUsername %s%s%s is taken by other user%s\n",
                RED, CYAN, username, RED, RESET);

        if (!memcmp(reply, "SUC", 3))
            break;
    }

    printf("User %s[%s]%s login successfully\n\n", CYAN, username, RESET);

    // shared memory
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    static char *send = NULL;
    send = mmap(NULL, BUFSIZE, prot, flags, -1, 0);
    pid_t cpid = 1;

    while (1)
    {
        memset(reply, 0, BUFSIZE);

        // conditional forking skill
        if (cpid)
            cpid = fork();
        else
            cpid = 1;

        switch (cpid)
        {
        case 0:
            while (1)
            {
                memset(send, 0, BUFSIZE);
                for (int i = 0; i < BUFSIZE; i++)
                {
                    // scanf command
                    if ((inputc = getc(stdin)) != EOF)
                    {
                        if (inputc == 10)
                        {
                            send[i] = '\0';
                            break;
                        }
                        send[i] = inputc;
                    }
                }
                if (!strcmp(send, "help"))
                    listCmds();
                else
                    break;
            };
            write(fd, send, BUFSIZE);
            exit(1);
        default:
            // non-blocking wait
            cpid = waitpid(-1, NULL, WNOHANG);
            while (1)
            {
                memset(reply, 0, BUFSIZE);
                read(fd, reply, BUFSIZE);
                if (!memcmp(reply, "END", 3))
                    break;
                printf("%s", reply);
            }
            break;
        case -1:
            printf("Fork failed.\n");
            exit(1);
        }

        if (!strcmp(send, "bye"))
            break;
    }
    munmap(send, BUFSIZE);
}

void listCmds()
{
    printf("\n    %shelp%s    >> %s get all available commands for helping.\n", PURPLE, RESET, YELLOW);
    printf("    %slist%s    >> %s get all registered users list.\n", PURPLE, RESET, YELLOW);
    printf("     %s...%s    >> %s broadcast this message to all on-line users.\n", PURPLE, RESET, YELLOW);
    printf("%s \\> user%s    >> %s unicast your message to that user.\n", PURPLE, RESET, YELLOW);
    printf("     %sbye%s    >> %s exit this chatting system.\n\n", PURPLE, RESET, YELLOW);
}

void showTime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf(YELLOW "%d-%02d-%02d %02d:%02d:%02d\n" YELLOW,
           tm.tm_year + 1900,
           tm.tm_mon + 1,
           tm.tm_mday,
           tm.tm_hour,
           tm.tm_min,
           tm.tm_sec);
}
