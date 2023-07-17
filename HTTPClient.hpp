#pragma once
#include "TCPClient.hpp"
#include "Packets.hpp"
struct HTTPClient :public TCPClient
{
	HTTPClient();
	HTTPClient(TCPSocket& sock);
	HTTPClient(int socket, sockaddr_in addr);
	int send(std::string message);
	int send(HTTPResponse& resp);
};