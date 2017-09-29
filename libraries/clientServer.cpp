#include "clientServer.h"

int mdnet::start()
{
	WSADATA wsaData; // Service structure for storing information about the implementation of Windows Sockets.
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {	// If an error occurred while loading the library.
		std::cerr << "WSAStartup failed: " << result << "\n";
	}
	return result;
}

void mdnet::out()
{
	WSACleanup();
}


mdnet::Client::Client()
{
	memset(&hints, 0, sizeof hints);
}

mdnet::Client::~Client()
{
	freeaddrinfo(clientInfo); // all done with this structure
	closesocket(sock);
}
void mdnet::Client::close()
{
	freeaddrinfo(clientInfo); // all done with this structure
	closesocket(sock);
}
//Initialize and connect the client to the server.
//-------------------------------------
int mdnet::Client::init(addrinfo info, const char *IP, const char *port)
{
	serverIP = IP;
	serverPort = port;

	// создание сокета
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		std::cerr << "Socket() error " << WSAGetLastError() << "\n";
		return SOCKET_ERROR;
	}
	hints = info;

	int rv;
	if ((rv = getaddrinfo(serverIP, serverPort, &hints, &clientInfo)) != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
		exit(1);
	}
	return sock;
}


//Returns the value of the connected socket, or -1 if unable to connect
int  mdnet::Client::аccession()
{
	addrinfo *p = NULL;
	int sockfd;
	// loop through all the results and connect to the first we can
	for (p = clientInfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("Not connected");
			closesocket(sock);
			continue;
		}
		break; // if we get here, we must have connected successfully
	}
	if (p == NULL) {
		// looped off the end of the list with no connection
		std::cerr << "failed to connect\n";
		return -1;
	}
	sock = sockfd;
	return sockfd;
}

int mdnet::Client::setSocket(int inSocket)
{
	sock = inSocket;
	return sock;
}

addrinfo *mdnet::Client::getInfo()
{
	return clientInfo;
}


//Creation of queues.
//-------------------------------------
bool mdnet::Client::addSendFile(const TCHAR *fileName)
{
	if (listSendFile.size() < 101) {
		listSendFile.push_back(fileName);
		return true;
	}
	else
		return false;
}

bool mdnet::Client::addRecvFile(const TCHAR *fileName)
{
	listRecvFile.push_front(fileName);
	for (; listRecvFile.size() > 100;) {
		listRecvFile.pop_back();
	}
	return true;
}

//bool mdnet::Client::addSendMes()
//{
//}
//
//bool mdnet::Client::addRecvMes()
//{
//}
//
///Working with threads.
//-------------------------------------
//int mdepc::Client::startSendFile()
//{
//}
//}
//int mdepc::Client::startRecvFile()
//{
//}
//}
//int mdepc::Client::startSendMes();
//int mdepc::Client::startRecvMes();
//
//int mdnet::Client::stopSendFile()
//{
//	return 0;
//}
//int mdnet::Client::stopRecvFile()
//{
//	return 0;
//}
//int mdnet::Client::stopSendMes();
//int mdnet::Client::stopRecvMes();

//Working with file.
//-------------------------------------
int mdnet::Client::sendFile(const TCHAR *fileName)
{
	if (fileName == NULL){
		if (!listSendFile.empty()) {
			fileName = *listSendFile.cbegin();
		}
		else{
			return -4;
		}
	}
	int client_buffer_size = MAX_CLIENT_BUFFER_SIZE;
	int result = 0;
	unsigned long int count;
	char *buf = new char[MAX_CLIENT_BUFFER_SIZE];
	std::ifstream fread;
//-------------------------------------
	try {
			fread.open(fileName, std::ios::binary);
			if (!fread.is_open()) {
				return -2;
			}
			fread.seekg(0, fread.end);
			const unsigned long int length = fread.tellg();
			fread.seekg(0, fread.beg);
			std::wstring header;
			header = splitFileName(fileName) + _T('\n') + std::to_wstring(length) + _T('\n');
			const char *c_header;
			c_header = reinterpret_cast<const char*>(header.c_str());
			//std::wcstombs(c_header, header.c_str(), header.length());
//-------------------------------------
			result = send(sock, c_header, header.length(), 0);
			if (result == SOCKET_ERROR) {
				delete[]buf;
				fread.close();
				return SOCKET_ERROR;
			}
			result = recv(sock, buf, MAX_CLIENT_BUFFER_SIZE, 0);
			if (result != SOCKET_ERROR && buf[0] == 'R') {// An error occurred while sending data.				 
				std::cerr << "File transfer...\n";
//_________________________________________________________________________________________________
				for (count = 0; count < length; count++) {
					if (length - count > MAX_CLIENT_BUFFER_SIZE) {
						client_buffer_size = MAX_CLIENT_BUFFER_SIZE;
					}
					else {
						client_buffer_size = length - count;
					}

					int c;
					int data_size;
					fread.seekg(count, fread.beg);
					for (data_size = 0; data_size < client_buffer_size; data_size++) {
						if (fread.bad()) {
							break;
						}
						c = fread.get();
						buf[data_size] = c;
					}
					count += data_size - 1;
					result = send(sock, buf, data_size, 0);
					if (result == SOCKET_ERROR) {
						delete[]buf;
						fread.close();
						return SOCKET_ERROR;
					}
					//Confirmation of packet transfer and size.
					//===================================================================
					result = recv(sock, buf, MAX_CLIENT_BUFFER_SIZE, 0);
					if (result == SOCKET_ERROR) {
						delete[]buf;
						fread.close();
						return SOCKET_ERROR;
					}
					if (count < length) {//The file will not read completely.
						if (buf[0] == '1' && result == 1);
						//std::cout << "ok! ";
						else {
							std::cerr << "Error writing file.\n" << std::endl;
							delete[]buf;
							fread.close();
							return -1;
						}
					}
					//===================================================================

				}
//Evaluation of the result of transmission.
//_________________________________________________________________________________________________
				fread.close();
				std::cout << "The file was created.\n";
				std::cout << "Send: " << count;
				result = recv(sock, buf, MAX_CLIENT_BUFFER_SIZE, 0);
				if (result < MAX_CLIENT_BUFFER_SIZE) {
					buf[result] = '\0';
				}
				if (result == SOCKET_ERROR) {
					delete[]buf;
					return SOCKET_ERROR;
				}
				else {
					std::cout << " | Recorder: " << buf << " bytes\n";
				}
			}
		else {
			std::cerr << "Error while transferring the file.\n" << std::endl;
			fread.close();
			delete[]buf;
			return -1;
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception in thread: " << e.what() << "\n";
		fread.close();
		delete[]buf;
		return -32;
	}
	delete[]buf;
	return count;
}





int mdnet::Client::recvFile(const TCHAR *directory)
{
	int result;
	int flag = mode;
	unsigned long int count = 0;
	char *buf = new char[MAX_CLIENT_BUFFER_SIZE];
	std::ofstream fwrite;
	try {
		result = recv(sock, buf, MAX_CLIENT_BUFFER_SIZE, 0);
		if (result < 0) {
			std::cerr << "Recv failed: " << WSAGetLastError() << "\n";
			result = send(sock, "Error", sizeof("Error"), 0);
			return -1;
		}
		const TCHAR *c_header = reinterpret_cast<const TCHAR*>(buf);

		//if (result < MAX_CLIENT_BUFFER_SIZE) {
		//	buf[result] = '\0';
		//}
		std::wstring fileName = _T("");
		std::string size = "";
		for (size_t i = 0, par = 0; i < result; i++) {
			if (buf[i] != '\n') {
				if (par == 0) {
					fileName += buf[i];
				}
				if (par == 1) {
					size += buf[i];
				}
			}
			else {
				par++;
			}
		}
		const unsigned long int length = std::stoull(size);
		if (directory == NULL) {
			directory = _T("/");
		}
		fileName = directory + fileName;
		addRecvFile(fileName.c_str());

		fwrite.open(fileName, std::ios::binary | flag);
		if (fwrite.fail()) {
			result = send(sock, "Error", sizeof("Error"), 0);
			return -2;
		}
		result = send(sock, "R", sizeof("R"), 0);
		if (result == SOCKET_ERROR) {
			// произошла ошибка при отправле данных
			std::cerr << "Send failed: " << WSAGetLastError() << "\n";
		}

		//_________________________________________________________________________________________________

		for (size_t numPack = 0; count < length; numPack++) {
			int result = recv(sock, buf, MAX_CLIENT_BUFFER_SIZE, 0);
			if (result > 0) {
				if (numPack > 0) {
					flag = std::ios::out | std::ios::app;
				}
				else {
					flag = std::ios::out | flag;
				}

				fwrite.write(buf, result);
				long int part = count;
				count = fwrite.tellp();
				part = count - part;
				//подтверждение о получении и корректной записи данных
				//=====================================================================================
				char *response_body;
				if (result == part) {
					// Отправляем ответ клиенту с помощью функции send
					response_body = "1";
				}
				else {
					response_body = "0";
				}
				// Отправляем ответ клиенту с помощью функции send
				result = send(sock, response_body, 1, 0);
				if (result == SOCKET_ERROR) {
					// произошла ошибка при отправле данных
					std::cerr << "send failed: " << WSAGetLastError() << "\n"; 
				}
			}
			else {
				delete[]buf;
				fwrite.close();
				return -1;
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << "Exception in thread: " << e.what() << "\n";
		delete[]buf;
		fwrite.close();
		return -4;
	}
//_________________________________________________________________________________________________
	delete[]buf;
	result = send(sock, std::to_string(count).c_str(), std::to_string(count).length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "Send failed: " << WSAGetLastError() << "\n";
	}

	fwrite.close();
	return count;
}

//int mdnet::Client::sendMessage(const char *Message);
//int mdnet::Client::recvMessage(const char *Message);



int mdnet::Client::setMode(int behavior)
{
	mode = behavior;
	return mode;
}


std::wstring mdnet::Client::splitFileName(const std::wstring& str)
{
	std::wstring fileName;
	size_t pos = str.find_last_of(_T("/\\"));
	if (pos != std::string::npos) {
		fileName = str.substr(pos + 1);
	}
	else {
		fileName = str;
	}
	return fileName;
}






//=================================================================================================






mdnet::Server::Server()
{
	memset(&hints, 0, sizeof hints);
}

mdnet::Server::~Server()
{
	freeaddrinfo(servInfo);
	closesocket(listenSocket);
}

//Initialize and connect the client to the server.
//-------------------------------------
int mdnet::Server::init(addrinfo info, const char *port, const char *IP)
{
	serverIP = IP;
	serverPort = port;
	hints = info;
	int result;

	//// Инициализируем структуру, хранящую адрес сокета - addr
	result = getaddrinfo(serverIP, serverPort, &hints, &servInfo);
	if (result != 0) {
		std::cerr << "getaddrinfo failed: " << gai_strerror(result) << "\n";
		return 1;
	}

	// Создание сокета
	listenSocket = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		std::cerr << "Error at socket: " << WSAGetLastError() << "\n";
		return 1;
	}

	// Привязываем сокет к IP-адресу
	result = bind(listenSocket, servInfo->ai_addr, (int)servInfo->ai_addrlen);
	if (result == SOCKET_ERROR) {
		std::cerr << "bind failed with error: " << WSAGetLastError() << "\n";
		return 1;
	}

	// Инициализируем слушающий сокет
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "listen failed with error: " << WSAGetLastError() << "\n";
		return 1;
	}
	return listenSocket;
}

//Returns the value of the connected socket, or -1 if unable to connect
int mdnet::Server::session()
{
	int client_socket = INVALID_SOCKET;
	// loop through all the results and bind to the first we can
	long int result;
	client_socket = accept(listenSocket, NULL, NULL);
	if (client_socket == INVALID_SOCKET) {
		std::cerr << "accept failed: " << WSAGetLastError() << "\n";
	}
	else {
		addClient(client_socket);
	}
	return client_socket;
}

bool mdnet::Server::addClient(SOCKET sock)
{
	if (listClients.size() < 101) {
		listClients.push_back(sock);
		return true;
	}
	return false;
}

SOCKET mdnet::Server::popClient()
{
	SOCKET soc = *listClients.begin();
	listClients.pop_front();
	return soc;

}