#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#define SLEEP_TIME 50
#define SLEEP_TIME_ONE 1

static const char *porocess_names[] = { " process\n", "1.First", "2.Second",
			"3.Third", "4.Fourth", "5.Fifth", "6.Sixth", "7.Seventh", "8.Eighth",
			"9.Ninth", "10.Tenth", "11.Eleventh", "12.Twelfth", "13.Thirteen",
			"14.Fourteenth", "15.Fifteenth", "16.Sixteenth", "17.Seventeenth",
			"18.Eighteenth", "19.Nineteenth", "20.Twentieth", };
#endif

#ifdef linux
#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define SLEEP_TIME 50000
#define SLEEP_TIME_ONE 1000
#define Sleep usleep
#define getch_noblock getchar
#define PROCESS_INFORMATION pid_t

typedef void *PVOID;
typedef PVOID HANDLE;
#define HANDLE PVOID

static const char *porocess_names[] = { " process   ", "1.First", "2.Second",
			"3.Third", "4.Fourth", "5.Fifth", "6.Sixth", "7.Seventh", "8.Eighth",
			"9.Ninth", "10.Tenth", "11.Eleventh", "12.Twelfth", "13.Thirteen",
			"14.Fourteenth", "15.Fifteenth", "16.Sixteenth", "17.Seventeenth",
			"18.Eighteenth", "19.Nineteenth", "20.Twentieth", };
#endif

using namespace std;

#define MAX_PROCESS_NUMBER 21
#define RADIX 10
