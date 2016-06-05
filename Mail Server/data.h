#pragma once
#include "libraries.h"

class Data {
private:
	char userName[CHAR_NAME_SIZE];
	char pathToInbox[CHAR_NAME_SIZE];
	char pathToOutbox[CHAR_NAME_SIZE];

public:
	Data(char *);
	~Data();

	void moveToInbox(char *, char *);
	void moveToOutbox(char *, char *);
	void inboxSend(SOCKET);
	void outboxSend(SOCKET);
};