#include "process.h"

PROCESS_INFORMATION process::start_process(wchar_t *name, int number_of_process) {
#ifdef _WIN32
	STARTUPINFO StartupInfo;

	char temp1[RADIX]; char temp2[RADIX] = "-n "; //переобразование в LTWSTR
	_itoa(number_of_process, temp1, RADIX);
	strcat(temp2, temp1);
	wchar_t *COMAND = GetWC(temp2);

	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ProcInfo, sizeof(ProcInfo));
	if (!CreateProcess(name, COMAND, NULL, NULL, FALSE, NULL, NULL, NULL, &StartupInfo, &ProcInfo))
		cout << "CreateProcess failed!" << endl;

	char temp3[RADIX];
	strcpy(temp3, temp1);
	strcat(temp3, temp1);
	strcat(temp3, temp1);
	CloseEvent = CreateEvent(NULL, FALSE, FALSE, GetWC(temp3));
	PrintEvent = CreateEvent(NULL, TRUE, FALSE, GetWC(temp1));
#endif
	return ProcInfo;
}

HANDLE process::start_print() {
	return PrintEvent;
}

HANDLE process::end_print() {
	return CloseEvent;
}
