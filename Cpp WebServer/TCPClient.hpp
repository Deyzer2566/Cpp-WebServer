#pragma once
#include "TCPSocket.hpp"
#include <string>
#include <memory>
struct TCPClient
{
public:

private:
	std::shared_ptr<TCPSocket> socket;

public:
	static const int packet_size = 4095;

	TCPClient(TCPSocket* sock);

	TCPClient(int socket, sockaddr_in addr);

	/*
	Записывает сообщение на сокет
	std::string message - строка, которую необходимо отправить
	bool guarantee - нужно ли гарантировать полное отправление пакета
	При удачном принятии пакета возвращает количество отправленных байт
	При ошибке возвращает системный код ошибки
	*/
	size_t send(std::string message);

	/*
	Чтение сообщения из сокета
	bool guarantee - необходимо ли полностью читать сообщение
	(например, когда размер принятого пакета > size_packet)
	При успешном принятии возвращает принятую строку
	При ошибке возвращает пустую строку
	*/
	std::string recv();

	void setBlocking(bool enable);

	bool operator==(TCPClient & client);
};
