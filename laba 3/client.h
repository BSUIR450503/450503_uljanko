#pragma once
#include "libraries.h"

class client {
private:
#ifdef _WIN32
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
	string message_to_print;
	string message_to_send;

#ifdef __linux__
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
	client();
#endif
#ifdef __linux__
	client(char *, char *, char *, char *, char *);
#endif
	void stop_client();
	void run_client_print();
	void run_client_send();
};