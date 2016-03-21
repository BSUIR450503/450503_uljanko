#pragma once
#include "libraries.h"

class server {
private:
#ifdef _WIN32
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ChildProcessInfo;
	HANDLE hPipe;

	HANDLE Semaphore_client_start_print;
	HANDLE Semaphore_client_end_print;
	HANDLE Semaphore_client_print_exit;

	HANDLE Semaphore_client_send;
	HANDLE Semaphore_client_send_exit;

	HANDLE Semaphore_server_start_print;
	HANDLE Semaphore_server_end_print;
	HANDLE Semaphore_server_print_exit;

	HANDLE Semaphore_server_send;
	HANDLE Semaphore_server_send_exit;

	char buff[BUFF_SIZE];
	bool flag;
#endif

	int size_of_buff;
	string message_to_send;
	string message_to_print;

#ifdef __linux__
	int pid;
	int shm_id;
	int sem_id;
	int sems_id;
	int sem_s_id;
	int sem_c_id;
	int NumberOfBlocks;
	int length;
#endif

public:
#ifdef _WIN32
	server(char *);
	void run_server_send();
#endif
#ifdef __linux__
	server();
	void run_server_send(char *);
#endif
	void run_server_print();
	void stop_server();
};