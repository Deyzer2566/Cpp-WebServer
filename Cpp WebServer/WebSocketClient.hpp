#pragma once
#include "TCPClient.hpp"
class WebSocketClient : public TCPClient
{
public:
	std::string webSocketScriptName;//��������� �������
	WebSocketClient(TCPClient client);
};
