#pragma once
#include "libraries.h"

class Client {
private:
	char clientName[CHAR_NAME_SIZE];
	SOCKET my_sock;

public:
	Client();
	~Client();

	bool initSokcets();
	bool initOpenSokcet();
	void connectToServer(char *);
	void startClient();
	void help();
	void getMessageFromServer(char *);
};