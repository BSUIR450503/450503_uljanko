#include "server.h"

#ifdef _WIN32
server::server(char *direction) {
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ChildProcessInfo, sizeof(ChildProcessInfo));

	size_of_buff = sizeof(buff);

	Semaphore_client_start_print = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_CLIENT_START_PRINT"));
	Semaphore_client_end_print = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_CLIENT_END_PRINT"));
	Semaphore_client_print_exit = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_CLIENT_PRINT_EXIT"));
	
	Semaphore_client_send = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_CLIENT_SEND"));
	Semaphore_client_send_exit = CreateSemaphore(NULL, 0, 1, GetWC("SEMAPHORE_CLIENT_SEND_EXIT"));

	cout << "Client started..." << endl << endl;

	hPipe = CreateNamedPipe(GetWC("\\\\.\\pipe\\Pipe"), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, INFINITE, (LPSECURITY_ATTRIBUTES)NULL);

	CreateProcess(GetWC(direction), GetWC("-n 2"), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &StartupInfo, &ChildProcessInfo);

	if (!ConnectNamedPipe(hPipe, (LPOVERLAPPED)NULL)) {
		cout << "Connection fail!\n";
	}

	Semaphore_server_start_print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_SERVER_START_PRINT"));
	Semaphore_server_end_print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_SERVER_END_PRINT"));
	Semaphore_server_print_exit = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_SERVER_PRINT_EXIT"));

	Semaphore_server_send = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_SERVER_SEND"));
	Semaphore_server_send_exit = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, GetWC("SEMAPHORE_SERVER_SEND_EXIT"));
}

void server::run_server_send() {
	HANDLE Semaphores[2];
	Semaphores[0] = Semaphore_server_send;
	Semaphores[1] = Semaphore_server_send_exit;
	while (1) {
		WaitForMultipleObjects(2, Semaphores, FALSE, INFINITE) - WAIT_OBJECT_0;

		ReleaseSemaphore(Semaphore_client_send, 1, NULL);
		if (getch_noblock() != -1) {
			DWORD number_of_bytes_written;
			print_client();
			cin.clear();
			getline(cin, message_to_send);

			if (message_to_send == "bye\0" || message_to_send == "quit\0") {
				ReleaseSemaphore(Semaphore_server_print_exit, 1, NULL);
				ReleaseSemaphore(Semaphore_client_print_exit, 1, NULL);
				ReleaseSemaphore(Semaphore_client_send_exit, 1, NULL);
				WaitForSingleObject(ChildProcessInfo.hProcess, INFINITE);
				break;
			}

			ReleaseSemaphore(Semaphore_client_start_print, 1, NULL);

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
			
			WaitForSingleObject(Semaphore_client_end_print, INFINITE);
		}
		Sleep(1);
	}
}

void server::run_server_print() {
	HANDLE Semaphores[3];
	Semaphores[0] = Semaphore_server_start_print;
	Semaphores[1] = Semaphore_server_end_print;
	Semaphores[2] = Semaphore_server_print_exit;
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
		print_server();
		for (int i = 0; i < size; i++) {
			cout << message_to_print[i];
			Sleep(SLEEP_TIME);
		}
		cout << endl;

		ReleaseSemaphore(Semaphore_server_end_print, 1, NULL);
	}
}

void server::stop_server() {
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
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)((Black << 4) | Green));
	cout << endl;
	system("pause");
	return;
}
#endif

#ifdef __linux__
union semun {
	int              val;
	struct semid_ds *buf;
	unsigned short  *array;
	struct seminfo  *__buf;
} semunion;

server::server() {
	size_of_buff = BUFF_SIZE;

	shm_id = shmget(IPC_PRIVATE, size_of_buff, IPC_CREAT | 0666);
	if (shm_id < 0) {
		printf("shmget error\n");
	}

	system("clear");

	sem_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
	semctl(sem_id, 0, SETALL, 0);
	if (sem_id < 0) {
		printf("Semaphores is not created.");
	}
	sems_id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
	semctl(sems_id, 0, SETALL, 0);
	if (sems_id < 0) {
		printf("Semaphores is not created.");
	}
	sem_s_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	semctl(sem_s_id, 0, SETALL, 0);
	if (sem_s_id < 0) {
		printf("Semaphores is not created.");
	}
	sem_c_id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	semctl(sem_c_id, 0, SETALL, 0);
	if (sem_c_id < 0) {
		printf("Semaphores is not created.");
	}

	cout << "Client started..." << endl << endl;

	pid = fork();
}

void server::run_server_send(char *argv) {
	switch (pid)
	{
	case -1:
		printf("New process is not created\n");
		return;

	case 0:
		execlp("gnome-terminal", "gnome-terminal", "-x", argv, "-n 2", itoa(shm_id),
			itoa(sem_id), itoa(sems_id), itoa(sem_s_id), itoa(sem_c_id), NULL);
		break;

	default:
	{
		void *buffer = shmat(shm_id, NULL, 0);
		while (1) {
			system("stty -echo");
			ReleaseSemaphore(sem_c_id, 0);
			WaitSemaphore(sem_s_id, 0);
			if (getch_noblock() != -1) {
				ReleaseSemaphore(sem_c_id, 1);
				message_to_send.clear();
				cout << "Client: ";
				system("stty echo");
				tcflush(STDIN_FILENO, TCIFLUSH);
				getline(cin, message_to_send);
				system("stty -echo");

				ReleaseSemaphore(sem_id, 2);

				if (message_to_send == "quit") {
					length = -1;
					*(int*)buffer = length;
					ReleaseSemaphore(sem_id, 0);
					ReleaseSemaphore(sems_id, 2);
					ReleaseSemaphore(sems_id, 0);
					ReleaseSemaphore(sem_c_id, 0);
					WaitSemaphore(sem_id, 1);
					waitpid(pid, NULL, 0);
					break;
				}

				length = message_to_send.size();
				*(int*)buffer = length;

				ReleaseSemaphore(sem_id, 0);
				WaitSemaphore(sem_id, 1);

				NumberOfBlocks = ceil((double)length / (double)size_of_buff);
				for (int i = 0; i < NumberOfBlocks; i++) {
					message_to_send.copy((char*)buffer, size_of_buff, i*size_of_buff);

					ReleaseSemaphore(sem_id, 0);
					WaitSemaphore(sem_id, 1);
				}
				WaitSemaphore(sem_id, 3);
			}
			usleep(SLEEP_TIME_ONE);
		}
		semctl(sem_id, 0, IPC_RMID, semunion);
	}
	break;
	}
}

void server::run_server_print() {
	void *buffer = shmat(shm_id, NULL, 0);
	while (1) {
		message_to_print.clear();

		WaitSemaphore(sems_id, 2);
		WaitSemaphore(sems_id, 0);

		length = *(int*)buffer;
		ReleaseSemaphore(sems_id, 1);

		if (length == -1) break;

		NumberOfBlocks = ceil((double)length / (double)size_of_buff);
		for (int i = 0; i < NumberOfBlocks; i++) {
			WaitSemaphore(sems_id, 0);
			message_to_print.append((char*)buffer, size_of_buff);
			ReleaseSemaphore(sems_id, 1);
		}

		message_to_print.resize(length);
		cout << "Server: ";

		for (int i = 0; i < length; i++) {
			putchar(message_to_print[i]);
			fflush(stdout);
			usleep(SLEEP_TIME);
		}
		cout << endl;

		ReleaseSemaphore(sems_id, 3);
	}
}

void server::stop_server() {
	system("clear");
	system("stty echo");
	return;
}
#endif