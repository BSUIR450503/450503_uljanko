#include "server.h"

Server::Server() {
	clientCount = 0;
}

Server::~Server() {
	if (Connections) {
		delete Connections;
	}
}

bool Server::initSokcets() {
	WSAData data;
	WORD version = MAKEWORD(2, 2);          // sokcet version
	int res = WSAStartup(version, &data);   // init socket
	if (res != 0) {
		cout << "Sockets error!";
		return false;
	}
	return true;
}

addrinfo *Server::initConnections() {
	struct addrinfo hints;
	struct addrinfo *result;

	// init memory for sockects
	Connections = (SOCKET*)calloc(SOCKET_MEM, sizeof(SOCKET));

	// clear the struct
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;        // socket family
	hints.ai_flags = AI_PASSIVE;      // socket flags
	hints.ai_socktype = SOCK_STREAM;  // socket type "tcp"
	hints.ai_protocol = IPPROTO_TCP;  // socket protocol

	getaddrinfo(NULL, PORT, &hints, &result); // get info about socket and server

	return result;
}

void Server::initConnectSokcets(addrinfo *result) {
	// init connect socket
	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);  // announce server
	listen(Listen, SOMAXCONN);    // wait time to connect
	freeaddrinfo(result);         // clear the result struct info
}

void Server::startServer() {
	cout << "Server started..." << endl;
	while (1) { // start server
		if (Connect = accept(Listen, NULL, NULL)) { // if connect is OK
			cout << "Client connected..." << endl;
			Connections[clientCount] = Connect;     // save client socket
													// send messege to client
			clientCount++;
			// create client thread
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)distributionMessages,
				this, NULL, NULL);
		}
		Sleep(SLEEP_TIME);
	}
}

void Server::distributionMessages(void *arr) {
	Data *userData = NULL;
	char *buffer;
	bool flag = false;
	int newMessage = 0;
	int currentUser = ((Server*)arr)->clientCount - 1;
	while (1) { // client started
		buffer = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));

		// if client have a message
		if (recv(((Server*)arr)->Connections[currentUser], buffer, CHAR_BUFFER_SIZE, NULL)) {
			if (flag == false) {
				strcpy_s(((Server*)arr)->clientsLogins[currentUser], buffer);
				flag = true;

				// create user data
				userData = new Data(((Server*)arr)->clientsLogins[currentUser]);
			}
			else {
				if ((int)buffer[0] == 0) {
					cout << "Client " << ((Server*)arr)->clientsLogins[currentUser]
						<< " disconnected!" << endl;
					break;
				}

				if (strcmp(buffer, COMMAND_SEE_INBOX) == 0 || strcmp(buffer, COMMAND_SEE_OUTBOX) == 0) {
					if (strcmp(buffer, COMMAND_SEE_INBOX) == 0) {
						((Server*)arr)->sendMessageToCient(COMMAND_SEE_INBOX,
							((Server*)arr)->Connections[currentUser], userData);
					}

					if (strcmp(buffer, COMMAND_SEE_OUTBOX) == 0) {
						((Server*)arr)->sendMessageToCient(COMMAND_SEE_OUTBOX,
							((Server*)arr)->Connections[currentUser], userData);
					}
				} else {
					FILE *temp;
					temp = fopen(TEMP_MESSAGE_FILE, "a+b");
					fprintf(temp, "%s", buffer);
					fclose(temp);
					newMessage++;
				}
			}
		}

		if (newMessage == NEW_MESSAGE) {
			newMessage = 0;
			FILE *receivedMessage;
			receivedMessage = fopen(TEMP_MESSAGE_FILE, "r");
			char from_user[CHAR_NAME_SIZE];
			char to_user[CHAR_NAME_SIZE];
			fscanf(receivedMessage, "%s", from_user);
			fscanf(receivedMessage, "%s", to_user);
			fclose(receivedMessage);

			cout << "Client " << from_user << " send message to " << to_user << "	";
			Data *temp = new Data(to_user);

			time_t td;
			td = time(NULL);
			cout << ctime(&td);

			SYSTEMTIME obj;
			GetSystemTime(&obj);
			char date[CHAR_NAME_SIZE];
			wsprintfA(date, "%02d_%02d_%02d", obj.wHour, obj.wMinute, obj.wSecond);
			strcat(date, ".txt");

			temp->moveToOutbox(from_user, date);
			temp->moveToInbox(to_user, date);

			remove(TEMP_MESSAGE_FILE);
			delete temp;
		}

		free(buffer);
		Sleep(SLEEP_TIME);
	}
}

void Server::sendMessageToCient(char *command, SOCKET socket, Data *userData) {
	if (strcmp(command, COMMAND_SEE_INBOX) == 0) {
		userData->inboxSend(socket);
	}
	if (strcmp(command, COMMAND_SEE_OUTBOX) == 0) {
		userData->outboxSend(socket);
	}
}