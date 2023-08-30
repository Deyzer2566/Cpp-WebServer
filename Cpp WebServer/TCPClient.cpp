#include "TCPClient.hpp"
#include <string>
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
#include "NonblockingException.hpp"
#include "ClosedSocketException.hpp"

/*
Конструктор сокета
*/
TCPClient::TCPClient(int socket, sockaddr_in addr)
{
    this->socket = std::shared_ptr<TCPSocket>(new TCPSocket(socket,addr));
}

TCPClient::TCPClient(TCPSocket* sock)
{
    this->socket = std::shared_ptr<TCPSocket>(sock);
}

/*
Записывает сообщение на сокет
std::string message - строка, которую необходимо отправить
При удачном принятии пакета возвращает количество отправленных байт
При ошибке возвращает системный код ошибки
*/
size_t TCPClient::send(std::string message)
{
	size_t sended_total = 0;
	do
	{
		size_t size = (message.size() - sended_total > packet_size) ? packet_size : (message.size() - sended_total);
		int s = ::send(socket.get()->getSocket(), &message[sended_total], size, 0);
		if (s == -1)
		{
			int code =
#ifdef _WIN32
				WSAGetLastError()
#else
				errno
#endif
				;
			if (
				code !=
#ifdef _WIN32
				WSAEWOULDBLOCK//Можно ли повторно отправить пакет
#else
				EAGAIN
#endif
				)
                throw code;
		}
		if (s == 0)
			throw ClosedSocketException();
		if (s > 0)
			sended_total += (size_t)s;
	} while (sended_total != message.size());
	return sended_total;
}

/*
Чтение сообщения из сокета
При успешном принятии возвращает принятую строку
При ошибке возвращает пустую строку
*/
std::string TCPClient::recv()
{
	char buffer[packet_size];
	std::string returned;
	int recieved_total = 0;
	do
	{
		int r = ::recv(socket.get()->getSocket(), buffer, packet_size, 0);
		if (r == -1)
		{
			int code =
#ifdef _WIN32
				WSAGetLastError()
#else
				errno
#endif
				;
            if (
                code ==
#ifdef _WIN32
                WSAEWOULDBLOCK//Пустой ли буфер
#else
                EAGAIN
#endif
                )
                throw NonblockingException();
			throw code;
		}
		if (r == 0)
            throw ClosedSocketException();
		if (r > 0)
		{
			recieved_total += r;
			returned.insert(returned.end(), buffer, buffer + r);
		}
	} while (recieved_total%packet_size == 0);
	return returned;
}

void TCPClient::setBlocking(bool enable)
{
    this->socket.get()->setBlocking(enable);
}

bool TCPClient::operator==(TCPClient & client)
{
    return this->socket == client.socket;
}
