#include "TCPSocket.hpp"
#ifdef _WIN32
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
Конструктор сокета
*/
TCPSocket::TCPSocket(int socket, sockaddr_in addr)
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

bool TCPSocket::setBlocking(bool enable)
{
    if (this->socket < 0) return false;

#ifdef _WIN32
    unsigned long mode = enable ? 0 : 1;
    return (ioctlsocket(this->socket, FIONBIO, &mode) == 0);
#else
    int flags = fcntl(this->socket, F_GETFL, 0);
    if (flags == -1) return false;
    flags = enable ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(this->socket, F_SETFL, flags) == 0);
#endif
}

int TCPSocket::getSocket()
{
	return socket;
}

sockaddr_in TCPSocket::getAddress()
{
	return addr;
}
