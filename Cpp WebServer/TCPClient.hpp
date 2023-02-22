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
	���������� ��������� ������
	*/
	State getState();

	/*
	���������� ��������� �� �����
	std::string message - ������, ������� ���������� ���������
	bool guarantee - ����� �� ������������� ������ ����������� ������
	��� ������� �������� ������ ���������� ���������� ������������ ����
	��� ������ ���������� ��������� ��� ������
	*/
	int send(std::string message, bool guarantee);

	/*
	������ ��������� �� ������
	bool guarantee - ���������� �� ��������� ������ ���������
	(��������, ����� ������ ��������� ������ > size_packet)
	��� �������� �������� ���������� �������� ������
	��� ������ ���������� ������ ������
	*/
	std::string recv(bool guarantee);
};