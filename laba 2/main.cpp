#include "process.h"
#include "libraries.h"

#ifdef _WIN32
int getch_noblock() //функция проверки нажития символа
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}
#endif

#ifdef linux
int flag_to_print = 0;
int flag_to_end = 1;

struct sigaction Signal_to_print;
struct sigaction Signal_to_end;

void set_print(int sig) {
	flag_to_print = 1;
}

void set_end(int sig) {
	flag_to_end = 1;
}
#endif

wchar_t *GetWC(const char *c) //преобразование char* в wchar_t
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

void print(int process_number) { //вывод дочерернего процесса
	for (int i = 0; i < strlen(porocess_names[process_number]); i++) {
		cout << porocess_names[process_number][i];
#ifdef linux
		fflush(stdout);
		refresh();
#endif
		Sleep(SLEEP_TIME);
	}
	for (int i = 0; i < strlen(porocess_names[0]); i++) {
		cout << porocess_names[0][i];
#ifdef linux
		fflush(stdout);
		refresh();
#endif
		Sleep(SLEEP_TIME);
	}
	return;
}

int main(int argc, char *argv[]) {

	if (argc == 2) { //уход на дочерний процесс
#ifdef _WIN32
		char temp3[RADIX];
		strcpy(temp3, argv[1]);
		strcat(temp3, argv[1]);
		strcat(temp3, argv[1]);
		HANDLE CanCloseEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, GetWC(temp3));
		HANDLE CanPrintEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, GetWC(argv[1]));

		while (1) {
			if (WaitForSingleObject(CanPrintEvent, 1) == WAIT_OBJECT_0) {
				print(atoi(argv[1]));
				ResetEvent(CanPrintEvent);
			}
			if (WaitForSingleObject(CanCloseEvent, 0) == WAIT_OBJECT_0) {
				CloseHandle(CanCloseEvent);
				CloseHandle(CanPrintEvent);
				return 0;
			}
		}
#endif
	}
	else { //уход на родительский процесс
		char symbol;
		int number_of_process = 0;
		bool flag = false;
		process Proc[MAX_PROCESS_NUMBER];

#ifdef _WIN32
		int currentNum = 1;
		system("cls");
#endif
#ifdef linux
		int currentNum = 0;
		PROCESS_INFORMATION mas[MAX_PROCESS_NUMBER];

		initscr();
		clear();
		noecho();
		refresh();

		Signal_to_print.sa_handler = set_print;
		sigaction(SIGUSR1,&Signal_to_print,NULL);
  
		Signal_to_end.sa_handler = set_end;
		sigaction(SIGUSR2,&Signal_to_end,NULL);
#endif

		while (1) { //мониторинг нажития и вызов функций
			symbol = getch_noblock();

			Sleep(SLEEP_TIME_ONE);
			switch (symbol) {
			case '+': {
				if (number_of_process < MAX_PROCESS_NUMBER - 1) {
#ifdef _WIN32
					number_of_process++;
					Proc[number_of_process].start_process(GetWC(argv[0]), number_of_process);
#endif
#ifdef linux
					mas[number_of_process] = fork();
					number_of_process++;
					
					switch (mas[number_of_process - 1])
					{
					case 0: {
						flag_to_end = 0;
						while (!flag_to_end) {
							Sleep(SLEEP_TIME_ONE);
							if (flag_to_print) {
								print(number_of_process);
								refresh();
								flag_to_print = 0;
								kill(getppid(),SIGUSR2);
							}
						}
						return 0;
						break;
						}
					case -1: {
						cout << "CreateProcess failed!" << endl;
						break;
					}
					default: {
						break;
					}
					}
#endif
				}
				break;
			}

			case '-': {
				if (number_of_process > 0) {
#ifdef _WIN32
					SetEvent(Proc[number_of_process].end_print());
#endif
#ifdef linux
					kill(mas[number_of_process - 1],SIGUSR2);
#endif
					number_of_process--;
				}
				break;
			}

			case 'q': {
				if (number_of_process > 0) {
#ifdef _WIN32
					for (int i = 1; i <= number_of_process; i++) {
						SetEvent(Proc[i].end_print());
					}
#endif
#ifdef linux
					for (; number_of_process >= 0; number_of_process--) {
						kill(mas[number_of_process - 1], SIGUSR2);
						waitpid(mas[number_of_process - 1],NULL,0);
					}
#endif
				}
#ifdef linux
				clear();  
				endwin();
#endif
				return 0;
			}
			}

#ifdef linux
			if (flag_to_end && number_of_process > 0) { //поочередный вывод
				flag_to_end = 0;
				if (currentNum >= number_of_process - 1) {
					currentNum = 0;
				}
				else {
					if (!flag) {
						currentNum++;
					}
				}
				flag = 0;
				kill(mas[currentNum],SIGUSR1);
			}
			refresh();
#endif
#ifdef _WIN32
			if (number_of_process > 0 && WaitForSingleObject(Proc[currentNum].start_print(), 0) == WAIT_TIMEOUT) {
				if (currentNum >= number_of_process) { //поочередный вывод
					currentNum = 1;
				}
				else {
					if (!flag) {
						currentNum++;
					}
				}
				flag = false;
				SetEvent(Proc[currentNum].start_print());
			}
#endif
		}
	}
}
