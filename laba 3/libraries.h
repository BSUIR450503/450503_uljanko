#pragma once
#ifdef _WIN32
#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#include <string>

#define BUFF_SIZE 10
#define SLEEP_TIME 30

wchar_t *GetWC(const char *);
void print_server();
void print_client();

enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};
#endif

#ifdef __linux__
#include <stdio.h>
#include <termios.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>
#include <math.h>

#define BUFF_SIZE 10
#define SLEEP_TIME 30000
#define SLEEP_TIME_ONE 1000

char *itoa(int);
void WaitSemaphore(int, int);
void ReleaseSemaphore(int, int);
#endif

using namespace std;

int getch_noblock();