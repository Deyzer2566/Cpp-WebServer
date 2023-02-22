#pragma once
#include "TCPClient.hpp"
#include <string>
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
TCPClient::TCPClient(){}

/*
Конструктор сокета
*/
TCPClient::TCPClient(int socket, sockaddr_in addr) :TCPSocket(socket,addr){}

TCPClient::TCPClient(TCPSocket& sock): TCPSocket(sock){}

TCPClient::~TCPClient()
{
	state = Disconnected;
}

/*
Возвращает состояние сокета
*/
TCPClient::State TCPClient::getState()
{
	return state;
}

/*
Записывает сообщение на сокет
std::string message - строка, которую необходимо отправить
bool guarantee - нужно ли гарантировать полное отправление пакета
При удачном принятии пакета возвращает количество отправленных байт
При ошибке возвращает системный код ошибки
*/
int TCPClient::send(std::string message, bool guarantee)
{
	int sended_total = 0;
	do
	{
		int size = (message.size() - sended_total > packet_size) ? packet_size : (message.size() - sended_total);
		int s = ::send(socket, &message[sended_total], size, 0);
		if (s == -1 && guarantee)
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
			{
				state = Error;
				return code;
			}
		}
		if (s == 0)
		{
			state = Disconnected;
			return 0;
		}
		if (s > 0)
			sended_total += s;
	} while (sended_total != message.size() && guarantee);
	state = Success;
	return sended_total;
}

/*
Чтение сообщения из сокета
bool guarantee - необходимо ли полностью читать сообщение
(например, когда размер принятого пакета > size_packet)
При успешном принятии возвращает принятую строку
При ошибке возвращает пустую строку
*/
std::string TCPClient::recv(bool guarantee)
{
	char buffer[packet_size];
	std::string returned;
	int recieved_total = 0;
	do
	{
		int r = ::recv(socket, buffer, packet_size, 0);
		if (r == -1 && guarantee)
		{
			int code =
#ifdef _WIN32
				WSAGetLastError()
#else
				errno
#endif
				;
			state = Error;
			return "";
			//				if (
			//					code == 
			//#ifdef _WIN32
			//					WSAEWOULDBLOCK//Пустой ли буфер
			//#else
			//					EAGAIN
			//#endif
			//					)
			//				{
			//					state=Error;
			//					return "";
			//				}
#ifdef _WIN32
			if (code == WSAECONNRESET)
			{
				state = Disconnected;
				return "";
			}
#endif
		}
		if (r == 0)
		{
			state = Disconnected;
			return "";
		}
		if (r > 0)
		{
			recieved_total += r;
			returned.insert(returned.end(), buffer, buffer + r);
		}
	} while (recieved_total%packet_size == 0 && guarantee);
	state = Success;
	return returned;
}