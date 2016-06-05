#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <time.h>

using namespace std;

#define PORT "1010"
#define _PORT 1010
#define SOCKET_MEM 64
#define SLEEP_TIME 50
#define CHAR_BUFFER_SIZE 1024
#define CHAR_BUFFER_SIZE_MAX 8192
#define CHAR_NAME_SIZE 30
#define IP_BUFF_SIZE 15
#define MAX_CLIENTS_ON_SERVER 20
#define ONE_BYTE 1
#define NEW_MESSAGE 2

#define COMMAND_EXIT "exit"
#define COMMAND_SEND "send"
#define COMMAND_HELP "help"
#define COMMAND_SEE_INBOX "sin"
#define COMMAND_SEE_OUTBOX "sout"
#define COMMAND_END_OF_SEND "endOfSend"

#define SEND_MESSAGE_FILE "send.txt"
#define TEMP_MESSAGE_FILE "temp.txt"