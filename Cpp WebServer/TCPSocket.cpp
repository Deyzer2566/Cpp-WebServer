#pragma once
#include "TCPSocket.hpp"
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
/*
Дефолтный конструктор
*/
TCPSocket::TCPSocket(){}

/*
Конструктор сокета
*/
TCPSocket::TCPSocket(int socket, sockaddr_in addr)
{
	create(socket, addr);
}

void TCPSocket::create(int socket, sockaddr_in addr)
{
	this->socket = socket;
	this->addr = addr;
}

TCPSocket::~TCPSocket()
{
	this->close();
}

/*
Закрывает соединение
*/
void TCPSocket::close()
{
#ifdef _WIN32
	::closesocket(socket);
#else
	::close(socket);
#endif
}

bool TCPSocket::setBlockingEnabled(bool blocking)
{
	if (this->socket < 0) return false;

#ifdef _WIN32
	unsigned long mode = blocking ? 0 : 1;
	return (ioctlsocket(this->socket, FIONBIO, &mode) == 0);
#else
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return false;
	flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	return (fcntl(fd, F_SETFL, flags) == 0);
#endif
}

void TCPSocket::swap(TCPSocket & sock)
{
	int tmp_sock = sock.socket;
	sockaddr_in tmp_addr = sock.addr;

	sock.socket = socket;
	sock.addr = addr;

	socket = tmp_sock;
	tmp_addr = tmp_addr;
}

int TCPSocket::getSocket()
{
	return socket;
}

sockaddr_in TCPSocket::getAddress()
{
	return addr;
}