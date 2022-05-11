#include <TcpLib.h>

int main(int argc, char *argv[])
{
    int port = handleArgs("SERVER", argc, argv);
    int srv_fd = ServerInit(port);
    MultiProc(srv_fd);

    return 0;
}
