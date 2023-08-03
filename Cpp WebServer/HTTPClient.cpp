#pragma once
#include "HTTPClient.hpp"
#include <string>
#include <thread>
#include "Packets.hpp"
HTTPClient::HTTPClient(int socket, sockaddr_in addr) :TCPClient(socket, addr){}

HTTPClient::HTTPClient(TCPSocket& sock) : TCPClient(sock){}

HTTPClient::HTTPClient(){}

int HTTPClient::send(std::string message)
{
	return TCPClient::send(message, true);
}

int HTTPClient::send(HTTPResponse & resp)
{
	resp.compile_packet(false);
	int r = send(resp.to_string());
	if (getState() == TCPClient::Error)
		return r;
	char b[TCPSocket::packet_size];
	for (size_t i = resp.file.tellg(); i < resp.sizeResponse; i += TCPSocket::packet_size)
	{
		size_t count = (resp.sizeResponse - i > TCPSocket::packet_size) ? TCPSocket::packet_size : (resp.sizeResponse - i);
		resp.file.read(b, count);
		r += send(std::string(b, b + count));
		if (getState() == TCPClient::Error)
			return r;
	}
	return r;
}