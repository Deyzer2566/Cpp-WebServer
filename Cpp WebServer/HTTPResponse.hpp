#pragma once
#include <string>
#include "HTTPPacket.hpp"
class HTTPResponse: public HTTPPacket
{
private:
	static const std::string version;
	unsigned short code;
	std::string message;
public:
    std::string getMessage();
    void setMessage(std::string message);
    void setCode(unsigned short code);
    unsigned short getCode();
    void setHeader(std::string key, std::string value);
    void setMessageBody(std::string messageBody);
	void operator=(HTTPResponse & packet);
	void compile_packet();
};
