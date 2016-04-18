#pragma once
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <process.h>
#include <comdef.h>
#include <string>

using namespace std;

#define EVENT_FINISH_WRITE 2
#define EVENT_FINISH_READ 0
#define EVENT_EXIT 1

#elif __linux__
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <aio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#endif

#define MAX_STRING_SIZE 255