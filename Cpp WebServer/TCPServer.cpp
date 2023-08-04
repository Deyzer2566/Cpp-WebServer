#include "TCPServer.hpp"

int TCPServer::listen(int port)
{
	socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (socket == -1)
		return 1;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	if (::bind(socket, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		close();
		return 2;
	}
	if (::listen(socket, 0x100) == -1)
	{
		close();
		return 3;
	}
	return 0;
}

bool TCPServer::accept(TCPSocket& sock)
{
	sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	int acc = ::accept(this->socket, (sockaddr*)&client_addr, &client_addr_size);
	if (acc > 0)
	{
		sock.create(acc, client_addr);
		return true;
	}
	return false;
}
