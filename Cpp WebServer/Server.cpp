#include "Server.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "Packets.hpp"
#include "Crypto.hpp"
#include <iostream>
#include "Endian.hpp"
#include "HTTPClient.hpp"
Server::~Server()
{
	for (std::vector<std::shared_ptr<HTTPClient>>::iterator it = http_clients.begin(); it != http_clients.end(); it = disconnect(it));
	for (std::vector<std::shared_ptr<WebSocketClient>>::iterator it = websocket_clients.begin(); it != websocket_clients.end(); it = disconnect(it));
	listener.close();
}
Server::Server(int port) : listener()
{
	startup(port);
}
std::vector<std::shared_ptr<HTTPClient>>::iterator Server::disconnect(std::vector<std::shared_ptr<HTTPClient>>::iterator it)
{
	return http_clients.erase(it);
}
std::vector<std::shared_ptr<WebSocketClient>>::iterator Server::disconnect(std::vector<std::shared_ptr<WebSocketClient>>::iterator it)
{
	return websocket_clients.erase(it);
}
void Server::startup(int port)
{
	if (listener.listen(port) != 0)
		throw std::string("Can't start the server");
	listener.setBlockingEnabled(false);
}
void Server::cycle()
{
	std::shared_ptr<HTTPClient> client = std::make_shared<HTTPClient>();
	if (listener.accept(*client.get()))
	{
		client->setBlockingEnabled(false);
		http_clients.emplace_back(client);
	}
	std::vector<std::shared_ptr<HTTPClient>> http_client_copy(http_clients);
	size_t disconnected = 0;
	for (std::vector<std::shared_ptr<HTTPClient>>::iterator it = http_client_copy.begin(); it != http_client_copy.end(); it++)
	{
		std::string message;
		try
		{
		    message = it->get()->recv();
		}
		catch (std::string error)
		{
		    std::cout<<error<<std::endl;
		}
		if (it->get()->getState() == TCPClient::Disconnected)
		{
			disconnect(it - http_client_copy.begin() + http_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		if (message == "")
			continue;
        HTTPRequest packet;
        try
        {
            packet = HTTPRequest(message);
        }
        catch (std::string message)
        {
            std::cout<<message<<std::endl;
        }
        if (packet.headers["Connection"] == "Upgrade")
		{
			if (packet.headers["Upgrade"] == "websocket")
			{
				HTTPResponse resp;
				bool script_exists = webSocketHandlers.find(packet.uri) != webSocketHandlers.end();
				if (!script_exists)
				{
					resp.code = 404;
					resp.message = "Not found";
					resp.headers["Connection"] = "close";
				}
				else
				{
					resp.code = 101;
					resp.message = "Web Socket Protocol Handshake";
					resp.headers["Upgrade"] = "WebSocket";
					resp.headers["Connection"] = "Upgrade";
					resp.headers["WebSocket-Origin"] = packet.headers["Host"];
					resp.headers["WebSocket-Location"] = "ws://" + packet.headers["Host"] + "/" + packet.uri;
					std::vector<unsigned long> h = SHA1(packet.headers["Sec-WebSocket-Key"] + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					std::vector<unsigned char> bytes;
					for (unsigned long c : h)
					{
						c = Endian::swapBEH(c);
						bytes.insert(bytes.end(), (unsigned char*)&c, (unsigned char*)&c + sizeof(long));
					}
					resp.headers["Sec-WebSocket-Accept"] = base64(bytes);
				}
				resp.compile_packet();
				it->get()->send(resp);
				if (script_exists)
				{
					std::shared_ptr<WebSocketClient> new_websocket = \
						std::make_shared<WebSocketClient>();
					new_websocket->swap(*(it->get()));
					new_websocket->webSocketScriptName = packet.uri;
					websocket_clients.emplace_back(new_websocket);
					disconnect(it - http_client_copy.begin() + http_clients.begin()-disconnected);
					disconnected++;
                    webSocketHandlers.at(packet.uri).get()->onConnect(*(new_websocket.get()));
				}
				continue;
			}
		}
        for(std::vector<std::unique_ptr<HTTPHandler>>::iterator httpHandlersIter = httpHandlers.begin();httpHandlersIter != httpHandlers.end();httpHandlersIter++)
        {
            httpHandlersIter->get()->executeRequest(packet,*it->get());
        }
	}
	http_client_copy.clear();
	std::vector<std::shared_ptr<WebSocketClient>> websocket_clients_copy(websocket_clients);
	disconnected = 0;
	for (std::vector<std::shared_ptr<WebSocketClient>>::iterator it = websocket_clients_copy.begin(); it != websocket_clients_copy.end(); it++)
	{
		std::string message;
		try
		{
		    message = it->get()->recv();
		}
		catch (std::string error)
		{
		    std::cout<<error<<std::endl;
		}
		if (it->get()->getState() == TCPClient::Disconnected)
		{
			disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		if (message == "")
			continue;
		WebSocketPacket packet(message);
		if (packet.OpCode == 8)
		{
		    webSocketHandlers.at(it->get()->webSocketScriptName)->onDisconnect(*it->get());
			disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		webSocketHandlers.at(it->get()->webSocketScriptName)->onMessage(packet,*it->get());
	}
	websocket_clients_copy.clear();
}
void Server::addHTTPHandler(HTTPHandler* httpHandler)
{
    this->httpHandlers.push_back(std::unique_ptr<HTTPHandler>(httpHandler));
}
void Server::addWebSocketHandler(std::string uri, WebSocketHandler* webSocketHandler)
{
    this->webSocketHandlers.insert(std::make_pair(uri,std::unique_ptr<WebSocketHandler>(webSocketHandler)));
}
