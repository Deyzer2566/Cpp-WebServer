#include "Server.hpp"
#include <vector>
#include <array>
#include <map>
#include <string>
#include <memory>
#include "HTTPResponse.hpp"
#include "HTTPRequest.hpp"
#include "Crypto.hpp"
#include <iostream>
#include "Endian.hpp"
#include "HTTPClient.hpp"
#include "NonblockingException.hpp"
#include "ClosedSocketException.hpp"
Server::~Server()
{
	for (std::vector<HTTPClient>::iterator it = http_clients.begin(); it != http_clients.end(); it = disconnect(it));
	for (std::vector<WebSocketClient>::iterator it = websocket_clients.begin(); it != websocket_clients.end(); it = disconnect(it));
}
Server::Server(int port) : listener()
{
	listener.listen(port);
	listener.setBlocking(false);
}
std::vector<HTTPClient>::iterator Server::disconnect(std::vector<HTTPClient>::iterator it)
{
	return http_clients.erase(it);
}
std::vector<WebSocketClient>::iterator Server::disconnect(std::vector<WebSocketClient>::iterator it)
{
	return websocket_clients.erase(it);
}
void Server::cycle()
{
    try
    {
        TCPClient client = listener.accept();
		client.setBlocking(false);
		http_clients.emplace_back(client);
	}
	catch(std::string)
	{

	}
	std::vector<HTTPClient> http_client_copy(http_clients);
	size_t disconnected = 0;
	for (std::vector<HTTPClient>::iterator it = http_client_copy.begin(); it != http_client_copy.end(); it++)
	{
		std::string message;
		try
		{
		    message = it->recv();
		}
		catch (int error)
		{
            disconnect(it - http_client_copy.begin() + http_clients.begin() - disconnected);
            disconnected++;
			continue;
		}
		catch (ClosedSocketException e)
		{
            disconnect(it - http_client_copy.begin() + http_clients.begin() - disconnected);
            disconnected++;
            continue;
		}
		catch (NonblockingException e)
		{
		    continue;
		}
        HTTPRequest packet;
        try
        {
            packet = HTTPRequest(message);
        }
        catch (std::string message)
        {
            std::cout<<message<<std::endl;
            continue;
        }
        if (packet.getHeader("Connection") == "Upgrade")
		{
			if (packet.getHeader("Upgrade") == "websocket")
			{
				HTTPResponse resp;
				bool script_exists = webSocketHandlers.find(packet.getURI()) != webSocketHandlers.end();
				if (!script_exists)
				{
					resp.setCode(404);
					resp.setMessage("Not found");
					resp.setHeader("Connection","close");
				}
				else
				{
					resp.setCode(101);
					resp.setMessage("Web Socket Protocol Handshake");
					resp.setHeader("Upgrade","WebSocket");
					resp.setHeader("Connection", "Upgrade");
					resp.setHeader("WebSocket-Origin", packet.getHeader("Host"));
					resp.setHeader("WebSocket-Location","ws://" + packet.getHeader("Host") + "/" + packet.getURI());
					std::array<unsigned long, 5> h = SHA1(packet.getHeader("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					std::vector<unsigned char> bytes;
					for (unsigned long c : h)
					{
						c = changeEndianIfNotBigEndian(c);
						bytes.insert(bytes.end(), (unsigned char*)&c, (unsigned char*)&c + sizeof(long));
					}
					resp.setHeader("Sec-WebSocket-Accept", base64(bytes));
				}
				resp.compile_packet();
				it->send(resp);
				if (script_exists)
				{
					WebSocketClient new_websocket(*it);
					new_websocket.webSocketScriptName = packet.getURI();
					websocket_clients.push_back(new_websocket);
					disconnect(it - http_client_copy.begin() + http_clients.begin()-disconnected);
					disconnected++;
                    webSocketHandlers.at(packet.getURI()).get()->onConnect(new_websocket);
				}
				continue;
			}
		}
        for(std::vector<std::unique_ptr<HTTPHandler>>::iterator httpHandlersIter = httpHandlers.begin();httpHandlersIter != httpHandlers.end();httpHandlersIter++)
        {
            httpHandlersIter->get()->executeRequest(packet,*it);
        }
	}
	http_client_copy.clear();
	std::vector<WebSocketClient> websocket_clients_copy(websocket_clients);
	disconnected = 0;
	for (std::vector<WebSocketClient>::iterator it = websocket_clients_copy.begin(); it != websocket_clients_copy.end(); it++)
	{
		std::string message;
		try
		{
		    message = it->recv();
		}
		catch (int error)
		{
            disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		catch (ClosedSocketException e)
		{
            disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		catch (NonblockingException e)
		{
		    continue;
		}
		WebSocketPacket packet(message);
		if (packet.getOpcode() == WebSocketPacket::CLOSE)
		{
		    webSocketHandlers.at(it->webSocketScriptName)->onDisconnect(*it);
			disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		webSocketHandlers.at(it->webSocketScriptName)->onMessage(packet,*it);
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
