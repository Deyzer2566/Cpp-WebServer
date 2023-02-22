#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Script.hpp"
#include "HTTPClient.hpp"
#include "TCPServer.hpp"
#include "WebSocketClient.hpp"
struct Server
{
private:
	TCPServer listener;
	Script HTTPHandler;
	void send_from_lua(unsigned int id, std::string message);
	void disconnect_for_lua(unsigned int id);
	std::vector<std::shared_ptr<HTTPClient>>::iterator disconnect(\
		std::vector<std::shared_ptr<HTTPClient>> ::iterator it);
	std::vector<std::shared_ptr<WebSocketClient>>::iterator disconnect(\
		std::vector<std::shared_ptr<WebSocketClient>> ::iterator it);
	std::vector<std::shared_ptr<HTTPClient>> http_clients;
	std::vector<std::shared_ptr<WebSocketClient>> websocket_clients;
	std::string workDir;
	std::string cache;
public:
	Server();
	void cycle();
	int startup(int port);
	~Server();
	Server(std::string workDir,int port);
	std::map<std::string, std::shared_ptr<Script>> webSocketScripts;
	bool addWebSocketScript(std::string filename, std::string name);
};