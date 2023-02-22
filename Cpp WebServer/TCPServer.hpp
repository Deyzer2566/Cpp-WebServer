#pragma once
#include "TCPSocket.hpp"
struct TCPServer: public TCPSocket
{
	int listen(int port);

	bool accept(TCPSocket & sock);
};