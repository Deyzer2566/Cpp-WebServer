#pragma once
#include "TCPSocket.hpp"
#include <string>
struct TCPClient: public TCPSocket
{
public:
	enum State{ Success, Error, Disconnected };

private:
	State state;

public:
	static const int packet_size = 4095;
	
	TCPClient();

	TCPClient(TCPSocket& sock);

	TCPClient(int socket, sockaddr_in addr);

	~TCPClient();

	/*
	Возвращает состояние сокета
	*/
	State getState();

	/*
	Записывает сообщение на сокет
	std::string message - строка, которую необходимо отправить
	bool guarantee - нужно ли гарантировать полное отправление пакета
	При удачном принятии пакета возвращает количество отправленных байт
	При ошибке возвращает системный код ошибки
	*/
	int send(std::string message, bool guarantee);

	/*
	Чтение сообщения из сокета
	bool guarantee - необходимо ли полностью читать сообщение
	(например, когда размер принятого пакета > size_packet)
	При успешном принятии возвращает принятую строку
	При ошибке возвращает пустую строку
	*/
	std::string recv(bool guarantee);
};