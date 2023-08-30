#include "TCPServer.hpp"

void TCPServer::listen(int port)
{
	int sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		throw std::string("Cant initialize the socket");
	sockaddr_in addr;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	socket = std::shared_ptr<TCPSocket>(new TCPSocket(sock,addr));
	if (::bind(socket.get()->getSocket(), (sockaddr*)&addr, sizeof(addr)) == -1)
		throw std::string("Cant bind the socket");
	if (::listen(socket.get()->getSocket(), 0x100) == -1)
		throw std::string("Cant listen connections to the socket");
}

TCPClient TCPServer::accept()
{
	sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	int acc = ::accept(this->socket.get()->getSocket(), (sockaddr*)&client_addr, &client_addr_size);
	if (acc > 0)
	{
		return TCPClient(acc,client_addr);
	}
	throw std::string("No connections");
}

void TCPServer::setBlocking(bool enable)
{
    this->socket.get()->setBlocking(enable);
}
