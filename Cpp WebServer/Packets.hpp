#pragma once
#include <vector>
#include <string>
#include <map>
#ifdef _DEBUG
#include <iostream>
#endif
#include <fstream>
struct HTTPPacket
{
	std::string startingLine;
	std::map<std::string, std::string>headers;
	std::string messageBody;
	std::string to_string();
};
struct HTTPRequest: public HTTPPacket
{
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> params;
	bool success;
	HTTPRequest();
	void operator=(HTTPRequest packet);
	HTTPRequest(std::string _X);
};
struct HTTPResponse: public HTTPPacket
{
	size_t sizeResponse;
	std::ifstream file;
	static const std::string version;
	unsigned short code;
	std::string message;
	bool success;
	HTTPResponse();
	void operator=(HTTPResponse & packet);
	HTTPResponse(HTTPRequest packet);
	~HTTPResponse();
	void compile_packet(bool appendBody);
};
struct WebSocketPacket
{
	bool Fin;
	bool RSV[3];
	bool isMask;
	unsigned char Mask[4];
	unsigned char OpCode;
	unsigned long long Length;
	std::string data;
	void read(std::string message);
	void create(int OpCode, bool useMask, std::string data);
	std::string toArray();
	WebSocketPacket();
	WebSocketPacket(std::string message);
};