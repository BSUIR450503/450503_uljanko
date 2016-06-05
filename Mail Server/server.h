#pragma once
#include "libraries.h"
#include "data.h"

class Server {
private:
	SOCKET Connect;       // main socket
	SOCKET *Connections;  // sockets array of all users
	SOCKET Listen;        // socket to connect
	int clientCount;      // number of clients on server
	char clientsLogins[MAX_CLIENTS_ON_SERVER][CHAR_NAME_SIZE];

public:
	Server();
	~Server();

	bool initSokcets();
	addrinfo *initConnections();
	void initConnectSokcets(addrinfo *);
	void startServer();
	static void distributionMessages(void *);
	void sendMessageToCient(char *, SOCKET, Data *);
};