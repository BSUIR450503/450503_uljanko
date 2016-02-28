#pragma once
#include "libraries.h"

wchar_t *GetWC(const char *);

class process {
private:
	PROCESS_INFORMATION ProcInfo;
	HANDLE PrintEvent;
	HANDLE CloseEvent;

public:
	PROCESS_INFORMATION start_process(wchar_t *, int);
	HANDLE start_print();
	HANDLE end_print();
};
