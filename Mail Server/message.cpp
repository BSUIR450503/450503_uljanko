#pragma once
#include "message.h"

Message::Message(char *sender) {
	from_user = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
	to_user = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
	message_text = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));

	strcpy(from_user, sender);

	cout << endl << "	Input addressee: ";
	cin >> to_user;
	while (cin.get() != '\n');

	cout << "	Input message text: ";
	cin >> message_text;
	int count = strlen(message_text);
	char buff;
	while (1) {
		buff = cin.get();
		if (buff == '\n') {
			break;
		}
		message_text[count] = buff;
		count++;
	}
}

Message::~Message() {
	free(from_user);
	free(to_user);
	free(message_text);
}

void Message::convert() {
	FILE *sendMessage;
	sendMessage = fopen(SEND_MESSAGE_FILE, "w");

	fprintf(sendMessage, "%s\n", from_user);
	fprintf(sendMessage, "%s\n", to_user);
	fprintf(sendMessage, "%s\n", message_text);

	fclose(sendMessage);
}

void Message::sendMessage(SOCKET socket) {
	FILE *sendFile;
	sendFile = fopen(SEND_MESSAGE_FILE, "r+b");

	char buffer[ONE_BYTE];
	while (true) {
		if (fread(&buffer, ONE_BYTE, ONE_BYTE, sendFile) == 0) break;
		send(socket, buffer, ONE_BYTE, NULL);
	}

	fclose(sendFile);
	remove(SEND_MESSAGE_FILE);
}