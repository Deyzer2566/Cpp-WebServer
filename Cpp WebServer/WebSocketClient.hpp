#pragma once
#include "TCPClient.hpp"
struct WebSocketClient : public TCPClient
{
	std::string webSocketScriptName;//временное решение
	WebSocketClient();
	WebSocketClient(TCPSocket& sock);
	WebSocketClient(int socket, sockaddr_in addr);
};
