#include "data.h"

Data::Data(char *userName) {
	CreateDirectoryA(userName, NULL);

	char buff_in[CHAR_NAME_SIZE];
	strcpy_s(buff_in, userName);
	char buff_out[CHAR_NAME_SIZE];
	strcpy_s(buff_out, userName);
	strcat(buff_in, "/inbox");
	strcat(buff_out, "/outbox");

	CreateDirectoryA(buff_out, NULL);
	CreateDirectoryA(buff_in, NULL);

	strcpy(pathToInbox, buff_in);
	strcpy(pathToOutbox, buff_out);
}

Data::~Data() {}

void Data::moveToInbox(char *to_user, char *date) {
	FILE *temp;
	FILE *toInbox;
	char path[CHAR_NAME_SIZE];
	char buffer;

	strcpy(path, to_user);
	strcat(path, "/inbox/");
	strcat(path, date);

	temp = fopen(TEMP_MESSAGE_FILE, "r+b");
	toInbox = fopen(path, "w+b");

	while (true) {
		if (fread(&buffer, 1, 1, temp) == 0) break;
		fwrite(&buffer, 1, 1, toInbox);
	}

	fclose(temp);
	fclose(toInbox);
}

void Data::moveToOutbox(char *from_user, char *date) {
	FILE *temp;
	FILE *toOutbox;
	char path[CHAR_NAME_SIZE];
	char buffer;

	strcpy(path, from_user);
	strcat(path, "/outbox/");
	strcat(path, date);

	temp = fopen(TEMP_MESSAGE_FILE, "r+b");
	toOutbox = fopen(path, "w+b");

	while (true) {
		if (fread(&buffer, 1, 1, temp) == 0) break;
		fwrite(&buffer, 1, 1, toOutbox);
	}

	fclose(temp);
	fclose(toOutbox);
}

void Data::inboxSend(SOCKET socket) {
	string folder(((const char*)pathToInbox));
	folder.append("\\");
	string fileMask = folder + "*";
	char FileRead[MAX_PATH];
	bool flag = false;

	char *result;
	WIN32_FIND_DATA FindFileData;
	HANDLE find_Handle;
	find_Handle = FindFirstFile(fileMask.c_str(), &FindFileData);
	send(socket, "\nMail inbox:\n", strlen("\n	Mail inbox:\n"), NULL);
	if (find_Handle != INVALID_HANDLE_VALUE) {
		do {
			result = strchr(FindFileData.cFileName, '.txt');
			if (result > 0) {
				flag = true;
				char *bufferForFromUser = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
				char *bufferForToUser = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));

				FILE *inboxMail;
				strcpy(FileRead, folder.c_str());
				strcat(FileRead, FindFileData.cFileName);
				inboxMail = fopen(FileRead, "r");

				send(socket, "	From user:  ", strlen("From user:  "), NULL);

				fscanf(inboxMail, "%s\n", bufferForFromUser);
				fscanf(inboxMail, "%s\n", bufferForToUser);
				send(socket, bufferForFromUser, strlen(bufferForFromUser), NULL);
				send(socket, "\n			", strlen("\n			"), NULL);

				while (!feof(inboxMail)) {
					char *bufferForMessage = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));
					fscanf(inboxMail, "%s\n", bufferForMessage);
					bufferForMessage[strlen(bufferForMessage)] = ' ';
					send(socket, bufferForMessage, strlen(bufferForMessage), NULL);
					free(bufferForMessage);
				}
				send(socket, "\n", ONE_BYTE, NULL);
				
				fclose(inboxMail);

				free(bufferForFromUser);
				free(bufferForToUser);
			}
		} while (FindNextFile(find_Handle, &FindFileData) != 0);
		FindClose(find_Handle);
	}

	if (flag == false) {
		send(socket, "		No mail!\n", strlen("		No mail!\n"), NULL);
	}
}

void Data::outboxSend(SOCKET socket) {
	string folder(((const char*)pathToOutbox));
	folder.append("\\");
	string fileMask = folder + "*";
	char FileRead[MAX_PATH];
	bool flag = false;

	char *result;
	WIN32_FIND_DATA FindFileData;
	HANDLE find_Handle;
	find_Handle = FindFirstFile(fileMask.c_str(), &FindFileData);
	send(socket, "\nMail outbox:\n", strlen("\nMail outbox:\n"), NULL);
	if (find_Handle != INVALID_HANDLE_VALUE) {
		do {
			result = strchr(FindFileData.cFileName, '.txt');
			if (result > 0) {
				flag = true;
				char *bufferForFromUser = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));
				char *bufferForToUser = (char*)calloc(CHAR_NAME_SIZE, sizeof(char));

				FILE *inboxMail;
				strcpy(FileRead, folder.c_str());
				strcat(FileRead, FindFileData.cFileName);
				inboxMail = fopen(FileRead, "r");

				send(socket, "	To user:  ", strlen("To user:  "), NULL);

				fscanf(inboxMail, "%s\n", bufferForFromUser);
				fscanf(inboxMail, "%s\n", bufferForToUser);
				send(socket, bufferForToUser, strlen(bufferForToUser), NULL);
				send(socket, "\n			", strlen("\n			"), NULL);

				while (!feof(inboxMail)) {
					char *bufferForMessage = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));
					fscanf(inboxMail, "%s\n", bufferForMessage);
					bufferForMessage[strlen(bufferForMessage)] = ' ';
					send(socket, bufferForMessage, strlen(bufferForMessage), NULL);
					free(bufferForMessage);
				}
				send(socket, "\n", ONE_BYTE, NULL);

				fclose(inboxMail);

				free(bufferForFromUser);
				free(bufferForToUser);
			}
		} while (FindNextFile(find_Handle, &FindFileData) != 0);
		FindClose(find_Handle);
	}

	if (flag == false) {
		send(socket, "		No mail!\n", strlen("		No mail!\n"), NULL);
	}
}