#pragma once
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif
struct TCPSocket
{
protected:
	int socket;
	sockaddr_in addr;

public:
	/*
	Конструктор сокета
	*/
	TCPSocket(int socket, sockaddr_in addr);

	~TCPSocket();

	/*
	Закрывает сокет
	*/
	void close();

	/*
	Устанавливает блокирующий режим
	*/
	bool setBlocking(bool enable);

	int getSocket();

	sockaddr_in getAddress();
};
