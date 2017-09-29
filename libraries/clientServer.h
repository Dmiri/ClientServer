#pragma once
#ifndef CLIENT_SERVER
#define CLIENT_SERVER

#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tchar.h>

#pragma comment(lib, "Ws2_32.lib")

namespace mdnet
{
	int start();
	void out();


	class Client
	{
	public:
		Client();
		~Client();

//Initialize and connect the client to the server.
//-------------------------------------
		int init(addrinfo info, const char *IP = "121.0.0.1", const char *port = "8000");

		//Returns the value of the connected socket, or -1 if unable to connect
		int аccession();
		int setSocket(int inSocket);


//Creation of queues and control of queue length (up to 100 elements).
//-------------------------------------
		//Create a queue for sending files and control 
		//the length of the queue(up to 100 elements).
		bool addSendFile(const TCHAR *fileName);
		//Create a queue for sending files and control 
		//the length of the queue(up to 100 elements)
		bool addRecvFile(const TCHAR *fileName);
		//bool addSendMes(const char *);
		//bool addRecvMes(const char *);

///Working with threads.
//-------------------------------------
		//int startSendFile();
		//int startRecvFile();
		//int startSendMes();
		//int startRecvMes();
		//
		//int stopSendFile();
		//int stopRecvFile();
		//int stopSendMes();
		//int stopRecvMes();

//Sending and processing data.
//-------------------------------------
		//Возвращяет количество считаных байт
		int sendFile(const TCHAR *fileName);
		int recvFile(const TCHAR *directory);
		//int sendMessage(const char *Message);
		//int recvMessage(const char *Message);

		addrinfo *getInfo();
		//Set the file recording mode.
		int setMode(int behavior = std::ios::trunc);

		std::wstring splitFileName(const std::wstring& str);
		//Closes this socket
		void close();



	private:
		SOCKET sock;
		struct addrinfo hints, *clientInfo;
		int mode = std::ios::trunc;
		const int MAX_CLIENT_BUFFER_SIZE = 10240;
		const char *serverPort;
		const char *serverIP;
		std::list<const TCHAR*> listSendFile;
		std::list<const TCHAR*> listRecvFile;
		//std::list<const char*> sendMessage;
		//std::list<const char*> recvMessage;
	};


//=================================================================================================

class Server
{
public:
	Server();
	~Server();

	//Initialize and connect the client to the server.
	//-------------------------------------
	int init(addrinfo info, const char *port = "8000", const char *IP = NULL);

	//Returns the value of the connected socket, or -1 if unable to connect
	int session();
	bool addClient(SOCKET);
	SOCKET popClient();

private:
	SOCKET listenSocket;
	std::list<SOCKET> listClients;
	struct addrinfo hints, *servInfo = NULL;
	const char *serverPort;
	const char *serverIP;

};

}
#endif // !CLIENT_SERVER
