#include <tcplib.h>

int main(int argc, char *argv[])
{
    int fd = initServer();
    clientAcception(fd);

    return 0;
}
