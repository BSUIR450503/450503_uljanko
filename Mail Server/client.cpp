#include "client.h"
#include "message.h"

Client::Client() {
	cout << "Input your login: ";
	cin >> clientName;
}

Client::~Client() {}

bool Client::initSokcets() {
	WSAData data;
	WORD version = MAKEWORD(2, 2);          // sokcet version
	int res = WSAStartup(version, &data);   // init socket
	if (res != 0) {
		cout << "Sockets error!";
		return false;
	}
	return true;
}

bool Client::initOpenSokcet() {
	my_sock = socket(
		AF_INET,        // for Web-app
		SOCK_STREAM,    // thread
		IPPROTO_TCP     //ÒÑÐ-protocol
	);

	if (my_sock == -1) {
		cout << "Sockets error!";
		return false;
	}
}

void Client::connectToServer(char *ip) {
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;              // Wep-app family
	dest_addr.sin_port = htons(_PORT);           // connected port
	dest_addr.sin_addr.s_addr = inet_addr(ip);   // set ip address

	// make connection
	connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr));
}

void Client::startClient() {
	cout << "Client started...";
	// send client name to server
	send(my_sock, clientName, strlen(clientName), NULL);

	while (1) {
		char *buffer = (char*)calloc(CHAR_BUFFER_SIZE, sizeof(char));
		cout << "\nEnter command: ";
		cin >> buffer;
		while (cin.get() != '\n');
		
		// clouse client
		if (strcmp(buffer, COMMAND_EXIT) == 0) {
			free(buffer);
			break;
		}

		// send message to server
		if (strcmp(buffer, COMMAND_SEND) == 0) {
			Message *message = new Message(clientName);
			message->convert();
			message->sendMessage(my_sock);
			delete message;
		}

		// client help
		if (strcmp(buffer, COMMAND_HELP) == 0) {
			help();
		}

		// see all mail in inbox
		if (strcmp(buffer, COMMAND_SEE_INBOX) == 0) {
			getMessageFromServer(COMMAND_SEE_INBOX);
		}

		// see all mail in outbox
		if (strcmp(buffer, COMMAND_SEE_OUTBOX) == 0) {
			getMessageFromServer(COMMAND_SEE_OUTBOX);
		}
		
		free(buffer);
		Sleep(SLEEP_TIME);
	}
}

void Client::getMessageFromServer(char *command) {
	char *buffer;
	send(my_sock, command, strlen(command), NULL);

	for (int i = 0; i < NEW_MESSAGE; i++) {
		buffer = (char*)calloc(CHAR_BUFFER_SIZE_MAX, sizeof(char));
		
		if (recv(my_sock, buffer, CHAR_BUFFER_SIZE_MAX, NULL) > 0) {
			if (strcmp(buffer, COMMAND_END_OF_SEND) == 0) {
				break;
			}
			cout << buffer;
		}

		free(buffer);
		Sleep(SLEEP_TIME);
	}
}

void Client::help() {
	cout << endl << "sin      -- show mail inbox";
	cout << endl << "sout     -- show mail outbox";
	cout << endl << "send     -- write mail and send";
	cout << endl << "exit     -- close programm" << endl;
}