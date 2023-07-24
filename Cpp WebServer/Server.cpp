#pragma once
#include "Server.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "Script.hpp"
#include <LuaBridge/LuaBridge.h>
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
Server::Server(std::string workDir, int port) :workDir(workDir), listener()
{
	startup(port);
}
void Server::send_from_lua(unsigned int id, std::string message)
{
	WebSocketPacket pack;
	pack.create(0x01, 0, message.c_str());
	websocket_clients[id]->send(pack.toArray());
}
void Server::disconnect_for_lua(unsigned int id)
{
	WebSocketPacket pack;
	pack.create(0x08, 0, "");
	websocket_clients[id]->send(pack.toArray());
}
bool Server::addWebSocketScript(std::string filename, std::string name)
{
	std::shared_ptr<Script> scr = std::make_shared<Script>();
	if (scr->init(filename))
	{
		std::cout << "Не найден файл скрипта с названием " << filename << std::endl;
		return false;
	}
	if (!luabridge::getGlobal(scr->getState(), "onConnect").isFunction() &&
		!luabridge::getGlobal(scr->getState(), "onDisconnect").isFunction() &&
		!luabridge::getGlobal(scr->getState(), "onMessage").isFunction())
	{
		std::cout << "В файле скрипта " << filename << " не найдены onConnect, onDisconnect или onMessage!" << std::endl;
		return false;
	}
	std::function<void(unsigned int, std::string)> send_for_lua_func = std::bind(&Server::send_from_lua, this, std::placeholders::_1, std::placeholders::_2);
	luabridge::getGlobalNamespace(scr->getState()).addFunction("Send", send_for_lua_func);
	std::function<void(unsigned int)> disconnect_for_lua_func = std::bind(&Server::disconnect_for_lua, this, std::placeholders::_1);
	luabridge::getGlobalNamespace(scr->getState()).addFunction("Disconnect", disconnect_for_lua_func);
	webSocketScripts.emplace(name, scr);
	return true;
}
std::vector<std::shared_ptr<HTTPClient>>::iterator Server::disconnect(std::vector<std::shared_ptr<HTTPClient>>::iterator it)
{
	return http_clients.erase(it);
}
std::vector<std::shared_ptr<WebSocketClient>>::iterator Server::disconnect(std::vector<std::shared_ptr<WebSocketClient>>::iterator it)
{
	return websocket_clients.erase(it);
}
int Server::startup(int port)
{
	if (listener.listen(80) != 0)
		return -1;
	listener.setBlockingEnabled(false);
	if (HTTPHandler.init("HTTPHandler.lua"))
	{
		std::cout << luabridge::LuaException(HTTPHandler.getState(), 0).what() << std::endl;

		return -1;
	}
	return 0;
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
		std::string message = it->get()->recv(true);
		if (it->get()->getState() == TCPClient::Disconnected)
		{
			disconnect(it - http_client_copy.begin() + http_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		if (message == "")
			continue;
		HTTPRequest packet(message);
		if (!packet.success)
			continue;
		if (packet.uri == "/")
			packet.uri += "index";
		luabridge::LuaRef table(HTTPHandler.getState());
		table = luabridge::newTable(HTTPHandler.getState());
		table["method"] = packet.method.c_str();
		table["uri"] = packet.uri.c_str();
		table["params"] = luabridge::newTable(HTTPHandler.getState());
		for (std::map<std::string, std::string>::iterator it = packet.params.begin(); it != packet.params.end(); it++)
			table["params"][it->first.c_str()] = it->second.c_str();
		table["headers"] = luabridge::newTable(HTTPHandler.getState());
		for (std::map<std::string, std::string>::iterator it = packet.headers.begin(); it != packet.headers.end(); it++)
			table["headers"][it->first.c_str()] = it->second.c_str();
		table["messageBody"] = std::string(packet.messageBody.begin(), packet.messageBody.end()).c_str();
		table["id"] = it->get()->getSocket();
		table["servdir"] = workDir.c_str();
		luabridge::LuaRef ret = HTTPHandler.call("onMessage", table);
		bool sendFile = false;
		bool response = false;
		if (ret.isTable())
		{
			sendFile = ret["sendFile"].cast<bool>();
			response = ret["response"].cast<bool>();
		}
		packet.uri.insert(packet.uri.begin(), workDir.begin(), workDir.end());
		if (packet.headers["Connection"] == "Upgrade")
		{
			if (packet.headers["Upgrade"] == "websocket")
			{
				std::string scriptName = packet.uri.substr(workDir.size() + 1);
				HTTPResponse resp;
				bool script_exists = webSocketScripts.find(scriptName) != webSocketScripts.end();
				if (!script_exists)
				{
					resp.code = 404;
					resp.message = "Not found";
					resp.headers["Connection"] = "keep-alive";
				}
				else
				{
					resp.code = 101;
					resp.message = "Web Socket Protocol Handshake";
					resp.headers["Upgrade"] = "WebSocket";
					resp.headers["Connection"] = "Upgrade";
					resp.headers["WebSocket-Origin"] = packet.headers["Host"];
					resp.headers["WebSocket-Location"] = "ws://" + packet.headers["Host"] + "/" + scriptName;
					std::vector<unsigned long> h = SHA1(packet.headers["Sec-WebSocket-Key"] + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
					std::vector<unsigned char> bytes;
					for (unsigned long c : h)
					{
						c = Endian::swapBEH(c);
						bytes.insert(bytes.end(), (unsigned char*)&c, (unsigned char*)&c + sizeof(long));
					}
					resp.headers["Sec-WebSocket-Accept"] = base64(bytes);
					luabridge::LuaRef l = luabridge::getGlobal(webSocketScripts[scriptName]->getState(), "onConnect");
					l = l(it->get()->getSocket());
				}
				resp.compile_packet(false);
				it->get()->send(resp);
				if (script_exists)
				{
					std::shared_ptr<WebSocketClient> new_websocket = \
						std::make_shared<WebSocketClient>();
					new_websocket->swap(*(it->get()));
					new_websocket->webSocketScriptName = scriptName;
					websocket_clients.emplace_back(new_websocket);
					disconnect(it - http_client_copy.begin() + http_clients.begin()-disconnected);
					disconnected++;
				}
				continue;
			}
		}
		if (!response)
			continue;
		HTTPResponse resp(packet);
		resp.headers["Accept-Ranges"] = "bytes";
		resp.headers["Connection"] = packet.headers["Connection"];
		resp.headers["Server"] = "DeyzServer_v0.5";
		if (!sendFile)
		{
			resp.code = ret["code"].cast<unsigned short>();
			resp.message = ret["message"].cast<std::string>();
			if (ret["cookies"].isTable())
			{
				ret["cookies"].push(HTTPHandler.getState());
				luabridge::push(HTTPHandler.getState(), luabridge::Nil());
				while (lua_next(HTTPHandler.getState(), -2))
				{
					std::string value = luabridge::LuaRef::fromStack(HTTPHandler.getState(), -1).cast<std::string>();
					std::string key = luabridge::LuaRef::fromStack(HTTPHandler.getState(), -2).cast<std::string>();
					resp.headers["Set-cookie"] += (key + "=" + value + "; ");
					lua_pop(HTTPHandler.getState(), 1);
				}
			}
			if (ret["headers"].isTable())
			{
				ret["headers"].push(HTTPHandler.getState());
				luabridge::push(HTTPHandler.getState(), luabridge::Nil());
				while (lua_next(HTTPHandler.getState(), -2))
				{
					std::string value = luabridge::LuaRef::fromStack(HTTPHandler.getState(), -1).cast<std::string>();
					std::string key = luabridge::LuaRef::fromStack(HTTPHandler.getState(), -2).cast<std::string>();
					resp.headers[key] = value;
					lua_pop(HTTPHandler.getState(), 1);
				}
			}
			if (ret["messageBody"].isString())
			{
				std::string str = ret["messageBody"].cast<std::string>();
				resp.messageBody.insert(resp.messageBody.begin(), str.begin(), str.end());
				resp.sizeResponse = str.size();
			}
		}
		//else
		//	resp = HTTPResponse(packet);
		resp.compile_packet(false);
		it->get()->send(resp);
	}
	http_client_copy.clear();
	std::vector<std::shared_ptr<WebSocketClient>> websocket_clients_copy(websocket_clients);
	disconnected = 0;
	for (std::vector<std::shared_ptr<WebSocketClient>>::iterator it = websocket_clients_copy.begin(); it != websocket_clients_copy.end(); it++)
	{
		std::string message = it->get()->recv(true);
		if (it->get()->getState() == TCPClient::Disconnected)
		{
			disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		if (message == "")
			continue;
		WebSocketPacket packet(message);
		if (packet.Fin == 0)
		{
			cache += packet.data;
			continue;
		}
		if (packet.Fin == 1 && cache.size() > 0)
		{
			packet.data += cache;
			cache.clear();
		}
		if (packet.OpCode == 8)
		{
			luabridge::getGlobal(webSocketScripts[it->get()->webSocketScriptName]->getState(), "onDisconnect")(it->get()->getSocket());
			disconnect(it - websocket_clients_copy.begin() + websocket_clients.begin() - disconnected);
			disconnected++;
			continue;
		}
		Script * scr = webSocketScripts[it->get()->webSocketScriptName].get();
		luabridge::getGlobal(scr->getState(), "onMessage")(it->get()->getSocket(), packet.data.c_str());
	}
	websocket_clients_copy.clear();
}
