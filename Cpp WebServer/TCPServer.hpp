#pragma once
#include "TCPSocket.hpp"
#include "TCPClient.hpp"
#include <memory>
class TCPServer
{
private:
    std::shared_ptr<TCPSocket> socket;
public:
	void listen(int port);

	TCPClient accept();

	void setBlocking(bool enable);
};
