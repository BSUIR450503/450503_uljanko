#pragma once
#include "libraries.h"

class Message {
private:
	char *from_user;
	char *to_user;
	char *message_text;

public:
	Message(char *);
	~Message();

	void convert();
	void sendMessage(SOCKET);
};