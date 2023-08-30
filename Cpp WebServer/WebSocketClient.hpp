#pragma once
#include "TCPClient.hpp"
class WebSocketClient : public TCPClient
{
public:
	std::string webSocketScriptName;//временное решение
	WebSocketClient(TCPClient client);
};
