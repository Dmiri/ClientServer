#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "clientServer.h"
#pragma comment(lib, "Ws2_32.lib")

#define PORT "8243"

int main()
{
	std::cerr << "\t\t\t SERVER\n";
	std::cerr << "\t   For the correct operation of the application, you need the \"resources\"\n"
		<< "\tfolder in the same directory as the server program.\n"
		<< "\tYou only need to use only Latin characters in the filename or directory.\n";
	const std::wstring directory = _T("resources/");
	int result;
	mdnet::start();
	mdnet::Server server;
	struct addrinfo hints, *addr = NULL;
	memset(&hints, NULL, sizeof hints);

	hints.ai_flags = AI_PASSIVE; // use my IP address
	hints.ai_family = AF_INET; // Determines that the network will be used to work with the socket
	hints.ai_socktype = SOCK_STREAM; // Set the stream type of the socket
	hints.ai_protocol = IPPROTO_TCP; // The TCP protocol is used

		result = server.init(hints, PORT);
	if (result < 0) {
		std::cerr << "Error initializing the socket.\n";
	}
	for (;;) {
		int client_socket = server.session();
		if (client_socket == INVALID_SOCKET) {
			std::cerr << "accept failed: " << WSAGetLastError() << "\n";
		}
		else {
			mdnet::Client client;
			int sock = client.setSocket(server.popClient());
			std::cerr << "Connection open ";
			client.recvFile(directory.c_str());
			std::cerr << " | Connection closed.\n";
		}
	}

	mdnet::out();
	return 0;
}