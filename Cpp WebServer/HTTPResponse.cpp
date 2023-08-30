#include "HTTPResponse.hpp"
const std::string HTTPResponse::version = "HTTP/1.1";
void HTTPResponse::operator=(HTTPResponse & packet)
{
	code = packet.code;
	message = packet.message;
	messageBody = packet.messageBody;
	headers = packet.headers;
	startingLine = packet.startingLine;
}
void HTTPResponse::compile_packet()
{
	startingLine = version + " " + std::to_string(code) + " " + message;
	headers["Content-Length"] = std::to_string(messageBody.size());
}
std::string HTTPResponse::getMessage()
{
    return message;
}
void HTTPResponse::setMessage(std::string message)
{
    this->message = message;
}
void HTTPResponse::setCode(unsigned short code)
{
    this->code = code;
}
unsigned short HTTPResponse::getCode()
{
    return code;
}
void HTTPResponse::setHeader(std::string key, std::string value)
{
    headers[key]=value;
}
void HTTPResponse::setMessageBody(std::string messageBody)
{
    this->messageBody = messageBody;
}
