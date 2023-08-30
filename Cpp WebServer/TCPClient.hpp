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
	���������� ��������� �� �����
	std::string message - ������, ������� ���������� ���������
	bool guarantee - ����� �� ������������� ������ ����������� ������
	��� ������� �������� ������ ���������� ���������� ������������ ����
	��� ������ ���������� ��������� ��� ������
	*/
	size_t send(std::string message);

	/*
	������ ��������� �� ������
	bool guarantee - ���������� �� ��������� ������ ���������
	(��������, ����� ������ ��������� ������ > size_packet)
	��� �������� �������� ���������� �������� ������
	��� ������ ���������� ������ ������
	*/
	std::string recv();

	void setBlocking(bool enable);

	bool operator==(TCPClient & client);
};
