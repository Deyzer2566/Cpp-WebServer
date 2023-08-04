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
	std::vector<std::shared_ptr<HTTPClient>>::iterator disconnect(\
		std::vector<std::shared_ptr<HTTPClient>> ::iterator it);
	std::vector<std::shared_ptr<WebSocketClient>>::iterator disconnect(\
		std::vector<std::shared_ptr<WebSocketClient>> ::iterator it);
	std::vector<std::shared_ptr<HTTPClient>> http_clients;
	std::vector<std::shared_ptr<WebSocketClient>> websocket_clients;
	std::vector<std::unique_ptr<HTTPHandler>> httpHandlers;
	std::map<std::string, std::unique_ptr<WebSocketHandler>> webSocketHandlers;
public:
	Server();
	void cycle();
	int startup(int port);
	~Server();
	Server(int port);
	void addHTTPHandler(HTTPHandler* httpHandler);
    void addWebSocketHandler(std::string uri, WebSocketHandler* webSocketHandler);
};
