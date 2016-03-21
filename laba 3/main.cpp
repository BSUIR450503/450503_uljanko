#include "libraries.h"
#include "client.h"
#include "server.h"

#ifdef _WIN32
int getch_noblock()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

wchar_t *GetWC(const char *c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void print_server() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)((Black << 4) | LightBlue));
	cout << "Server: ";
	SetConsoleTextAttribute(hConsole, (WORD)((Black << 4) | LightGray));
	return;
}

void print_client() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)((Black << 4) | LightCyan));
	cout << "Client: ";
	SetConsoleTextAttribute(hConsole, (WORD)((Black << 4) | LightGray));
	return;
}
#endif

#ifdef __linux__
char _getch() {
	char buf = 0;
	struct termios old = { 0 };
	fflush(stdout);
	if (tcgetattr(0, &old)<0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &old)<0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1)<0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if (tcsetattr(0, TCSADRAIN, &old)<0)
		perror("tcsetattr ~ICANON");

	return buf;
}

int _kbhit(void) {
	struct timeval tv;
	fd_set rdfs;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	FD_ZERO(&rdfs);
	FD_SET(STDIN_FILENO, &rdfs);

	select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &rdfs);

}

int getch_noblock()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

char *itoa(int i) {
	char *buff;
	buff = (char *)calloc(12, sizeof(char));
	char *ptr = buff + sizeof(buff) - 1;
	unsigned int u;
	int minus = 0;

	if (i < 0) {
		minus = 1;
		u = ((unsigned int)(-(1 + i))) + 1;
	}
	else {
		u = i;
		*ptr = 0;
	}
	do {
		*--ptr = '0' + (u % 10);
	} while (u /= 10);

	if (minus)
		*--ptr = '-';
	return ptr;
}

void WaitSemaphore(int sem_id, int num) {
	struct sembuf buf;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_num = num;
	semop(sem_id, &buf, 1);
}

void ReleaseSemaphore(int sem_id, int num) {
	struct sembuf buf;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_num = num;
	semop(sem_id, &buf, 1);
}
#endif

int main(int argc, char *argv[]) {
#ifdef _WIN32
	system("color 02");
#endif
	if (argc == 1) {
#ifdef _WIN32
		server Server(argv[0]);
		thread run_server_print(&server::run_server_print, Server);
		thread run_server_send(&server::run_server_send, Server);
		run_server_send.join();
		run_server_print.join();
		Server.stop_server();
#endif
#ifdef __linux__
		server Server;
		thread run_server_send(&server::run_server_send, Server, argv[0]);
		thread run_server_print(&server::run_server_print, Server);
		run_server_print.join();
		run_server_send.join();
		Server.stop_server();
#endif
	}
	else {
#ifdef _WIN32
		client Client;
		thread run_client_print(&client::run_client_print, Client);
		thread run_client_send(&client::run_client_send, Client);
		run_client_send.join();
		run_client_print.join();
		Client.stop_client();
#endif
#ifdef __linux__
		client Client(argv[2], argv[3], argv[4], argv[5], argv[6]);
		thread run_client_print(&client::run_client_print, Client);
		thread run_client_send(&client::run_client_send, Client);
		run_client_send.join();
		run_client_print.join();
		Client.stop_client();
#endif
	}
	return 0;
}