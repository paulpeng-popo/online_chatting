#define GREEN "\033[0;32m"
#define RED "\033[1;31m"
#define BLUE "\033[0;34m"
#define YELLOW "\033[1;33m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define RESET "\033[0m"

void help();
void welcome();
void showTime();
int handleArgs(char app_name[10], int argc, char *argv[]);
