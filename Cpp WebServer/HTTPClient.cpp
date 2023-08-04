#include "HTTPClient.hpp"
#include <string>
#include <thread>
#include "Packets.hpp"
HTTPClient::HTTPClient(int socket, sockaddr_in addr) :TCPClient(socket, addr){}

HTTPClient::HTTPClient(TCPSocket& sock) : TCPClient(sock){}

HTTPClient::HTTPClient(){}

int HTTPClient::send(HTTPResponse & resp)
{
	return this->TCPClient::send(resp.to_string());
}
