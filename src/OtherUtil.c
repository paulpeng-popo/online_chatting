#include <OtherUtil.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void help()
{
    printf("\n    %shelp%s    >> %s get all available commands for helping.\n", PURPLE, RESET, YELLOW);
    printf("    %slist%s    >> %s get all registered users list.\n", PURPLE, RESET, YELLOW);
    printf("     %s...%s    >> %s broadcast this message to all on-line users.\n", PURPLE, RESET, YELLOW);
    printf("%s \\> user%s    >> %s unicast your message to that user.\n", PURPLE, RESET, YELLOW);
    printf("     %sbye%s    >> %s exit this chatting system.\n\n", PURPLE, RESET, YELLOW);
}

void welcome()
{
    printf("%s\n", YELLOW);
    printf("Welcome to on-line chatting system\n");
    printf("Let's start gossiping with friends\n\n");
    printf("%s", RESET);
}

void showTime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf(YELLOW"%d-%02d-%02d %02d:%02d:%02d\n"YELLOW,
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec
    );
}

int handleArgs(char app_name[10], int argc, char *argv[])
{
    int port = -1;
    switch(argc)
    {
		case 1:
            if (!memcmp(app_name, "SERVER", 6)) printf("Specify the port to listen on: ");
            if (!memcmp(app_name, "CLIENT", 6)) printf("Input port num: ");
            scanf("%d", &port);
			break;
		case 2:
			port = atoi(argv[1]);
            break;
		default:
            printf("Detect redundant arguments.\n");
            printf("Machine shutting down.\n\n");
			exit(1);
	}

    return port;
}
