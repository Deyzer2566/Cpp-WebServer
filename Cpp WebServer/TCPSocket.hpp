#pragma once
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
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
	static const int packet_size = 4095;

	/*
	Дефолтный конструктор
	*/
	TCPSocket();

	void create(int socket, sockaddr_in addr);

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
	bool setBlockingEnabled(bool blocking);

	void swap(TCPSocket & sock);

	int getSocket();

	sockaddr_in getAddress();
};