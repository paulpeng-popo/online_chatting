#include <clienthelp.h>

int main(int argc, char *argv[])
{
	char ip_addr[] = "127.0.0.1";
	int port = 12345;
	int fd = initClient(ip_addr, port);
	startChat(fd);

	return 0;
}
