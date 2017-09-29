#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "clientServer.h"

#include <stdlib.h>
#include <locale>


#pragma comment(lib, "Ws2_32.lib")

void myExit(int error);

int main(int argc, char* argv[])
{
	char* current = setlocale(0, "");
	//std::locale current_locale("");
	//std::locale::global(current_locale);
	//SetConsoleCP(1251);
	//SetConsoleOutputCP(1251);

	std::cerr << "\t\t\t CLIENT\n";
	std::cerr << "\tYou only need to use only Latin characters in the filename or directory.\n";
	const std::string port = "8243";

	std::wstring fileName;
	std::string serveraddr;
	if (argc > 1) {
		serveraddr = argv[1];
	}
	else {
		myExit(-8);
	}

//Create a client and fill out the trail for transferring files.
//___________________________________________________________________
	mdnet::start();
	mdnet::Client client;
	int result;

	std::cout << "Enter the file name: ";
	std::wcin.clear();
	std::wcin.ignore(std::cin.rdbuf()->in_avail());
	////////////std::string c_fileName;
	////////////std::wcin.imbue(std::locale(""));
	//TCHAR c_fileName[100];
	//std::wcin.getline(c_fileName, 100);
	std::string c_fileName;
	std::getline(std::cin, c_fileName);
	//OemToChar();
	////////////std::wcin >> std::noskipws >> c_fileName;
	////////////mbstowcs(const_cast<TCHAR*>(fileName.c_str()), c_fileName.c_str(), c_fileName.length());
	std::ifstream fread(fileName, std::ios::binary);
	if (fread.is_open()) {
		client.addSendFile(fileName.c_str());
	}
	fread.close();

//Connected
//___________________________________________________________________
	addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // Determines that the network will be used to work with the socket
	hints.ai_protocol = IPPROTO_TCP; // The TCP protocol is used
	hints.ai_socktype = SOCK_STREAM; // Set the stream type of the socket

	result = client.init(hints, serveraddr.c_str(), port.c_str());
	if (result < 0) {
		std::cerr << "Error initializing the socket.\n";
		myExit(result);
	}

	result = client.àccession();
	if (result == SOCKET_ERROR) {
		std::cerr << "Connection not established.\n";
		myExit(result);
	}
//Transfer file.
//___________________________________________________________________

	result = client.sendFile(NULL);
	if (result < 0) {
		myExit(result);
	}

	mdnet::out();
	std::cin.clear();
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	std::cin.get();
	return 0;
}

void myExit(int error) {
	const char *msg = "";
	switch (error)
	{
	case -1:
		msg = ("Connection error: " + std::to_string(WSAGetLastError()) + "\n").c_str();
		break;
	case -2:
		msg = "Error reading the file.'\n'";
		break;
	case -4:
		msg = "The name of the file to send is not specified.";
		break;
	case -8:
		msg = "Server name is not specified.\n";
		break;
	case -32:
		msg = "An exception!";
		break;
	default:
		msg = "unknown";
		break;
	}
	mdnet::out();
	std::cerr << msg;
	std::cin.clear();
	std::cin.ignore(std::cin.rdbuf()->in_avail());
	std::cin.get();
	exit(error);
}