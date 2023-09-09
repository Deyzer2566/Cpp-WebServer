#pragma once
#include <string>
#include "HTTPPacket.hpp"
#include <cstdint>
class HTTPResponse: public HTTPPacket
{
private:
	static const std::string version;
	uint16_t code;
	std::string message;
public:
    std::string getMessage();
    void setMessage(std::string message);
    void setCode(uint16_t code);
    uint16_t getCode();
    void setHeader(std::string key, std::string value);
    void setMessageBody(std::string messageBody);
	void operator=(HTTPResponse & packet);
	void compile_packet();
};
