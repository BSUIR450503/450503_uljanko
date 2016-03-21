#include "client.h"

#ifdef _WIN32
client::client() {
	size_of_buff = sizeof(buff);

	Semaphore_client_start_print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_CLIENT_START_PRINT"));
	Semaphore_client_end_print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_CLIENT_END_PRINT"));
	Semaphore_client_print_exit = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_CLIENT_PRINT_EXIT"));

	Semaphore_client_send = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_CLIENT_SEND"));
	Semaphore_client_send_exit = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_CLIENT_SEND_EXIT"));

	Semaphore_server_start_print = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_SERVER_START_PRINT"));
	Semaphore_server_end_print = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_SERVER_END_PRINT"));
	Semaphore_server_print_exit = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_SERVER_PRINT_EXIT"));

	Semaphore_server_send = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_SERVER_SEND"));
	Semaphore_server_send_exit = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_SERVER_SEND_EXIT"));

	cout << "Server started..." << endl << endl;

	hPipe = CreateFile(GetWC("\\\\.\\pipe\\Pipe"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
}

void client::run_client_print() {
	HANDLE Semaphores[3];
	Semaphores[0] = Semaphore_client_start_print;
	Semaphores[1] = Semaphore_client_end_print;
	Semaphores[2] = Semaphore_client_print_exit;
	while (1) {
		flag = TRUE;
		DWORD number_of_bytes_read;
		message_to_print.clear();

		int index = WaitForMultipleObjects(3, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (index == 2) break;

		int number_of_blocks;
		int size;
		if (!ReadFile(hPipe, &number_of_blocks, sizeof(number_of_blocks), &number_of_bytes_read, NULL)) break;
		if (!ReadFile(hPipe, &size, sizeof(size), &number_of_bytes_read, NULL)) break;
		for (int i = 0; i < number_of_blocks; i++) {
			flag = ReadFile(hPipe, buff, size_of_buff, &number_of_bytes_read, NULL);
			if (!flag) break;
			message_to_print.append(buff, size_of_buff);
		}
		if (!flag) break;

		message_to_print.resize(size);
		print_client();
		for (int i = 0; i < size; i++) {
			cout << message_to_print[i];
			Sleep(SLEEP_TIME);
		}
		cout << endl;

		ReleaseSemaphore(Semaphore_client_end_print, 1, NULL);
	}
}

void client::run_client_send() {
	HANDLE Semaphores[2];
	Semaphores[0] = Semaphore_client_send;
	Semaphores[1] = Semaphore_client_send_exit;
	while (1) {
		ReleaseSemaphore(Semaphore_server_send, 1, NULL);
		int index = WaitForMultipleObjects(2, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;
		if (index == 1) break;

		if (getch_noblock() != -1) {
			DWORD number_of_bytes_written;
			print_server();
			cin.clear();
			getline(cin, message_to_send);

			ReleaseSemaphore(Semaphore_server_start_print, 1, NULL);

			int number_of_blocks = message_to_send.size() / size_of_buff + 1;
			int size = message_to_send.size();
			WriteFile(hPipe, &number_of_blocks, sizeof(number_of_blocks), &number_of_bytes_written, (LPOVERLAPPED)NULL);
			WriteFile(hPipe, &size, sizeof(size), &number_of_bytes_written, (LPOVERLAPPED)NULL);
			for (int i = 0; i < number_of_blocks; i++) {
				message_to_send.copy(buff, size_of_buff, i * size_of_buff);
				if (!WriteFile(hPipe, buff, size_of_buff, &number_of_bytes_written, (LPOVERLAPPED)NULL)) {
					cout << "Write Error" << endl;
				}
			}
			WaitForSingleObject(Semaphore_server_end_print, INFINITE);
		}
		Sleep(1);
	}
}
#endif

void client::stop_client() {
#ifdef _WIN32
	CloseHandle(hPipe);
	CloseHandle(Semaphore_client_start_print);
	CloseHandle(Semaphore_client_end_print);
	CloseHandle(Semaphore_client_print_exit);
	CloseHandle(Semaphore_client_send);
	CloseHandle(Semaphore_client_send_exit);
	CloseHandle(Semaphore_server_start_print);
	CloseHandle(Semaphore_server_end_print);
	CloseHandle(Semaphore_server_print_exit);
	CloseHandle(Semaphore_server_send);
	CloseHandle(Semaphore_server_send_exit);
#endif
	return;
}

#ifdef __linux__
client::client(char *argv_2, char *argv_3, char *argv_4,
	char *argv_5, char *argv_6) {
	size_of_buff = BUFF_SIZE;
	shm_id = atoi(argv_2);
	sem_id = atoi(argv_3);
	sems_id = atoi(argv_4);
	sem_s_id = atoi(argv_5);
	sem_c_id = atoi(argv_6);
	system("stty -echo");
	cout << "Server started..." << endl << endl;
}

void client::run_client_print() {
	void *buffer = shmat(shm_id, NULL, 0);
	while (1) {
		message_to_print.clear();

		WaitSemaphore(sem_id, 2);
		WaitSemaphore(sem_id, 0);

		length = *(int*)buffer;
		ReleaseSemaphore(sem_id, 1);

		if (length == -1) break;

		NumberOfBlocks = ceil((double)length / (double)size_of_buff);
		for (int i = 0; i < NumberOfBlocks; i++) {
			WaitSemaphore(sem_id, 0);
			message_to_print.append((char*)buffer, size_of_buff);
			ReleaseSemaphore(sem_id, 1);
		}

		message_to_print.resize(length);
		cout << "Client: ";

		for (int i = 0; i < length; i++) {
			putchar(message_to_print[i]);
			fflush(stdout);
			usleep(SLEEP_TIME);
		}
		cout << endl;

		ReleaseSemaphore(sem_id, 3);
	}
}

void client::run_client_send() {
	void *buffer = shmat(shm_id, NULL, 0);
	while (1) {
		system("stty -echo");
		ReleaseSemaphore(sem_s_id, 0);
		WaitSemaphore(sem_c_id, 0);
		if (getch_noblock() != -1) {
			ReleaseSemaphore(sem_s_id, 1);
			message_to_send.clear();
			cout << "Server: ";
			system("stty echo");
			tcflush(STDIN_FILENO, TCIFLUSH);
			getline(cin, message_to_send);
			system("stty -echo");

			ReleaseSemaphore(sems_id, 2);

			length = message_to_send.size();
			*(int*)buffer = length;

			ReleaseSemaphore(sems_id, 0);
			WaitSemaphore(sems_id, 1);

			NumberOfBlocks = ceil((double)length / (double)size_of_buff);
			for (int i = 0; i < NumberOfBlocks; i++) {
				message_to_send.copy((char*)buffer, size_of_buff, i*size_of_buff);

				ReleaseSemaphore(sems_id, 0);
				WaitSemaphore(sems_id, 1);
			}
			WaitSemaphore(sems_id, 3);
		}
		usleep(SLEEP_TIME_ONE);
		length = *(int*)buffer;
		if (length == -1) break;
	}
}
#endif