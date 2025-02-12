#define GREEN "\033[0;32m"
#define RED "\033[1;31m"
#define BLUE "\033[0;34m"
#define YELLOW "\033[1;33m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

int initServer();
void initSharedMemory();
void closeSharedMemory();
void clientAcception(int fd);
int clientRelay(int fd, char ip_addr[]);

int online(int fd, char username[], char ip_addr[]);
int offline(int index);
void synch(int fd, int index);
void bcast(char username[], char notif[]);
void ucast(int index, char target[], char msg[]);
void statusCmd(int fd);
