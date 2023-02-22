#pragma once
#include "WebSocketClient.hpp"
#include <string>
#include <thread>
#include "Packets.hpp"
WebSocketClient::WebSocketClient(int socket, sockaddr_in addr) :TCPClient(socket, addr){}

WebSocketClient::WebSocketClient(TCPSocket& sock) : TCPClient(sock){}

WebSocketClient::WebSocketClient(){}

int WebSocketClient::send(std::string message)
{
	return TCPClient::send(message, true);
}