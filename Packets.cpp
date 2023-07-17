#pragma once
#include "Packets.hpp"
#include <vector>
#include <string>
#include "Utilite.hpp"
#include <map>
#ifdef _DEBUG
#include <iostream>
#endif
#include <fstream>
#include "TCPSocket.hpp"

std::string HTTPPacket::to_string()
{
	std::string ret;
	ret += startingLine + "\n";;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		ret += it->first + ": " + it->second+"\n";
	ret += "\n";
	ret.insert(ret.end(),messageBody.begin(),messageBody.end());
	return ret;
}

HTTPRequest::HTTPRequest(){}
void HTTPRequest::operator=(HTTPRequest packet)
{
	method = packet.method;
	uri = packet.uri;
	version = packet.uri;
	params = packet.params;
	success = packet.success;
}
HTTPRequest::HTTPRequest(std::string _X)
{
	success = false;
	std::vector<std::string> strings = split(_X, "\r\n");
	if (strings.size() < 2)
	{
#ifdef _DEBUG
		std::cout << "Error in reading HTTP packet!" << std::endl << "Error in split of a packet!" << std::endl;
#endif
		return;
	}
	std::vector<std::string> startingLine = split(strings[0], " ");
	if (startingLine.size() != 3)
	{
#ifdef _DEBUG
		std::cout << "Error in reading HTTP packet!" << std::endl << "Error in split a starting line" << std::endl;
#endif
		return;
	}
	method = startingLine[0];
	uri = startingLine[1];
	if (uri.find("%") != uri.npos)
	{
		Unicode str = UTF_8_to_Unicode(uri, "%");
		uri.clear();
		unsigned short AB = 1040;
		unsigned short YAB = 1071;
		unsigned short A = 1072;
		unsigned short YA = 1103;
		for (unsigned short s : str){
			if (s >= AB&&s <= YAB)
				s -= (AB - 'À');
			else if (s >= A && s <= YA)
				s -= A - 'à';
			uri += char(s);
		}
	}
	this->startingLine = strings[0];
	if (uri.find("?") != uri.npos)
	{
		size_t index = uri.find('?');
		std::string splited = uri.substr(index + 1, uri.size() - index);
		uri = uri.substr(0, index);
		std::vector<std::string> params_and_meanings = split(splited, "&");
		for (size_t i = 0; i < params_and_meanings.size(); i++)
		{
			std::vector<std::string> splited1 = split(params_and_meanings[i], "=");
			if (splited1.size() == 2)
				params[splited1[0]] = splited1[1];
			else
			{
#ifdef _DEBUG
				std::cout << "Error in reading HTTP packet!" << std::endl << "Error in split a params" << std::endl;
#endif
				return;
			}
		}
	}
	version = startingLine[2];
	size_t i = 1;
	for (; i<strings.size() && strings[i] != ""; i++)
	{
		size_t pos = strings[i].find(": ");
		if (pos == strings[i].npos)
		{
#ifdef _DEBUG
			std::cout << "Error in reading HTTP packet!" << std::endl << "Error in split of headers[" << strings[i] << std::endl;
#endif
			return;
		}
		headers[strings[i].substr(0, pos)] = strings[i].substr(pos + 2);
	}
	i++;
	for (; i < strings.size(); i++)
	{
		messageBody.insert(messageBody.end(), strings[i].begin(), strings[i].end());
		if (i == strings.size() - 1)
			continue;
		messageBody.push_back('\r');
		messageBody.push_back('\n');
	}
	success = true;
}

const std::string HTTPResponse::version = "HTTP/1.1";
HTTPResponse::HTTPResponse() :code(0), sizeResponse(0), success(0){}
void HTTPResponse::operator=(HTTPResponse & packet)
{
	sizeResponse = packet.sizeResponse;
	if (packet.file.is_open())
		file.swap(packet.file);
	code = packet.code;
	message = packet.message;
	messageBody = packet.messageBody;
	headers = packet.headers;
	startingLine = packet.startingLine;
}
HTTPResponse::HTTPResponse(HTTPRequest packet) :code(0), sizeResponse(0), success(0)
{
	if (packet.version != HTTPResponse::version){
		code = 404;
		message = "not found!";
		return;
	}
	if (packet.method == "GET")
	{
		std::string filename = packet.uri;
		if (filename.find(".") == filename.npos)
		{
			const std::string formats[] = { ".html", ".htm" };
			for (int i = 0; i < 2; i++)
			{
				std::string fname = (filename + formats[i]);
				file.open(fname.c_str(), std::ios::in | std::ios::binary);
				if (file.is_open())
				{
					filename += formats[i];
					break;
				}
			}
		}
		else
			file.open(filename.c_str(), std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			code = 404;
			message = "Not Found";
			return;
		}
		file.seekg(0, file.end);
		sizeResponse = file.tellg();
		file.seekg(0, file.beg);
		{
			std::map<std::string, std::string>::iterator range = packet.headers.find("Range");
			int to = -1;
			if (range != packet.headers.end())
			{
				code = 206;
				message = "Partial Content";
				std::vector<std::string> spl = split(range->second, { "-" });
				if (spl.size() != 2)
				{
#ifdef _DEBUG
					std::cout << "Partial Content Error" << std::endl;
#endif
					message = "Bad Request";
					code = 400;
					sizeResponse = 0;
					return;
				}
				if (isStrInt(spl[0]) && isStrInt(spl[1]))
				{
					file.seekg(std::stoi(spl[0]));
					sizeResponse = std::stoi(spl[1]) - std::stoi(spl[0]);
				}
				else
				{
#ifdef _DEBUG
					std::cout << "Partial Content Error" << std::endl;
#endif
					message = "Bad Request";
					code = 400;
					sizeResponse = 0;
					return;
				}
			}
		}
		if (code == 0){
			code = 200;
			message = "OK";
		}
		headers["Server"] = "KozServer";
		headers["Accept-Ranges"] = "bytes";
		headers["Connection"] = packet.headers["Connection"];
	}
	else
	{
		code = 405;
		message = "Method Not Allowed";
	}
}
HTTPResponse::~HTTPResponse()
{
	file.close();
}
void HTTPResponse::compile_packet(bool appendBody)
{
	startingLine = version + " " + std::to_string(code) + " " + message;
	headers["Content-Length"] = std::to_string(sizeResponse);
	if (file.is_open() && appendBody)
	{
		char b[TCPSocket::packet_size];
		for (size_t i = file.tellg(); i < sizeResponse; i += TCPSocket::packet_size)
		{
			size_t count = (sizeResponse - i > TCPSocket::packet_size) ? TCPSocket::packet_size : (sizeResponse - i);
			file.read(b, count);
			messageBody.insert(messageBody.end(), b, b + count);
		}
	}
}

void WebSocketPacket::read(std::string message)
{
	int offset = 0;
	Fin = ((message[offset] & (0x80)) >> 7) & 0x01;
	RSV[0] = ((message[offset] & (0x40)) >> 6) & 0x01;
	RSV[1] = ((message[offset] & (0x20)) >> 5) & 0x01;
	RSV[2] = ((message[offset] & (0x10)) >> 4) & 0x01;
	OpCode = message[offset] & 0x0F;
	offset++;
	isMask = (message[offset] & (0x80)) >> 7;
	Length = message[offset] & (0x7F);
	offset++;
	if (Length == 126)
	{
		Length = ((message[offset] & 0xFF) << 8) | (message[offset + 1] & 0xFF);
		offset += 2;
	}
	else if (Length == 127)
	{
		Length = (message[offset] << 56) | (message[offset + 1] << 48) | (message[offset + 2] << 40) | (message[offset + 3] << 32) | (message[offset + 4] << 24) | (message[offset + 5] << 16) | (message[offset + 6] << 8) | message[offset + 7];
		offset += 8;
	}
	if (isMask)
	{
		Mask[0] = message[offset];
		Mask[1] = message[offset + 1];
		Mask[2] = message[offset + 2];
		Mask[3] = message[offset + 3];
		offset += 4;
	}
	data.resize(Length);
	for (size_t i = 0; i < Length; i++)
		data[i] = (message[i + offset]) ^ Mask[i % 4];
}
void WebSocketPacket::create(int OpCode, bool useMask, std::string data)
{
	this->OpCode = OpCode;
	Fin = 1;
	RSV[0] = 0; RSV[1] = 0; RSV[2] = 0;
	isMask = useMask;
	Mask[0] = Mask[1] = Mask[2] = Mask[3] = 0;
	if (isMask)
		for (int i = 0; i < 4; i++)
			Mask[i] = rand() % 256;
	Length = data.size();
	this->data = data;
}
std::string WebSocketPacket::toArray()
{
	std::string ret;
	ret.push_back((Fin & 1) << 7 | (RSV[0] & 1) << 6 | (RSV[1] & 1) << 5 | (RSV[2] & 1) << 4 | (OpCode & 0x0F));
	ret.push_back((isMask & 1) << 7);
	if (Length < 126)
		ret.back() |= Length & 0x7F;
	else
		if (Length <= 0xFFFF)
		{
			ret.back() |= 126;
			for (int i = 1; i >= 0; i--)
				ret.push_back(Length&(0xFF << (8 * i)));
		}
		else
		{
			ret.back() |= 127;
			for (int i = 3; i >= 0; i--)
				ret.push_back(Length&(0xFF << (8 * i)));
		}
	for (int i = 0; i < 4 && isMask; i++) {
		ret.push_back(Mask[i]);
	}
	for (size_t i = 0; i < Length; i++)
	{
		ret += data[i] ^ Mask[i % 4];
	}
	return ret;
}
WebSocketPacket::WebSocketPacket(){}
WebSocketPacket::WebSocketPacket(std::string message)
{
	read(message);
}