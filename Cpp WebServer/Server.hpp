#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "HTTPClient.hpp"
#include "TCPServer.hpp"
#include "WebSocketClient.hpp"
#include "HTTPHandler.hpp"
#include "WebSocketHandler.hpp"
struct Server
{
private:
	TCPServer listener;
	std::vector<HTTPClient>::iterator disconnect(\
		std::vector<HTTPClient> ::iterator it);
	std::vector<WebSocketClient>::iterator disconnect(\
		std::vector<WebSocketClient> ::iterator it);
	std::vector<HTTPClient> http_clients;
	std::vector<WebSocketClient> websocket_clients;
	std::vector<std::unique_ptr<HTTPHandler>> httpHandlers;
	std::map<std::string, std::unique_ptr<WebSocketHandler>> webSocketHandlers;
public:
	void cycle();
	~Server();
	Server(int port);
	void addHTTPHandler(HTTPHandler* httpHandler);
    void addWebSocketHandler(std::string uri, WebSocketHandler* webSocketHandler);
};
