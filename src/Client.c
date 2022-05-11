#include <TcpLib.h>

int main(int argc, char *argv[])
{
	char ip_addr[] = "127.0.0.1";
    int port = handleArgs("CLIENT", argc, argv);
	int cli_fd = ClientInit(ip_addr, port);
	Chatting(cli_fd);

    return 0;
}
