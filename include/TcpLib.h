#include <OtherUtil.h>

#define BUFSIZE 512
#define BACKLOG 5
#define MAXUSERS 5
#define RECORD 8

typedef struct UserNode {
    char name[BUFSIZE];
    char ip[16];
    int userfd;
    int active;
    char messaage[RECORD][BUFSIZE];
    int top;
} UserNode;

int ServerInit(int port);
void MultiProc(int fd);
int ServerWork(int fd, char ip_addr[]);
int RelayCLI(int fd, UserNode* shared_list, char ip_addr[]);

int ClientInit(char ip_addr[], int port);
void Chatting(int fd);

// void reaper(int sig);
void nodeClear(UserNode* shared_list);
int online(int fd, UserNode* shared_list, char username[], char ip_addr[]);
int offline(UserNode* shared_list, int index);
void synch(int fd, UserNode* shared_list, int index);
void bcast(UserNode* shared_list, char username[], char notif[]);
void ucast(UserNode* shared_list, int index, char target[], char msg[]);
void statusCmd(int fd, UserNode* shared_list);
