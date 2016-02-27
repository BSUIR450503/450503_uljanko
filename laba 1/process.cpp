#include "process.h"
#define NUM 10

const char *control_str = "123456789,";

process::process() {
	input_number = "<no init>";
	output_number = "<no init>";
}

process::process(string input) {


	input_number = input;
	output_number = "<no init>";
}

process::~process() {}

void process::show_number() {
	cout << endl << "Input number: " << input_number << endl;
	cout << "Answer (^2): " << output_number << endl << endl;
}

bool process::control() {
	int i = 0; int flag = 0;

	if (input_number[0] == ',') {
		return 1;
	}

	while (input_number[i] != '\0') {
		for (int j = 0; j < NUM; j++) {
			if (input_number[i] == control_str[j])
				flag = 1;
		}
		if (flag == 0)
			return 1;
		i++;
		flag = 0;
	}

	if (input_number[i - 1] == ',') {
		return 1;
	}

	return 0;
}

void process::start_process() {
	fstream input_file;
	input_file.open("temp", ios::app | ios::binary);
	input_file << input_number;
	input_file.close();

#ifdef _WIN32
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcInfo;
	TCHAR CommandLine[] = TEXT("processB");
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	ZeroMemory(&ProcInfo, sizeof(ProcInfo));

	if (!CreateProcess(NULL, CommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcInfo)) {
		cout << "CreateProcess failed!";
		return;
	}

	WaitForSingleObject(ProcInfo.hProcess, INFINITE );
	CloseHandle(ProcInfo.hProcess);
	CloseHandle(ProcInfo.hThread);
#endif

#ifdef linux
	pid_t pid; int status;
	switch(pid = fork()) {
	case -1:
		perror("fork");
		cout << "Error!";
		exit(1);
	case 0:
		execl("processB", "one", "two", "t", NULL);
		exit(0);
	default:
		wait(&status);
	}
#endif

	fstream output_file;
	output_file.open("temp", ios::in | ios::binary);
	output_file >> output_number;
	output_file.clear();
	output_file.sync();
	output_file.close();
	remove("temp");
}
