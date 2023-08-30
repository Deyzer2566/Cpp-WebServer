#pragma once
#include <string>
#include <map>
class HTTPPacket
{
protected:
	std::string startingLine;
	std::map<std::string, std::string>headers;
	std::string messageBody;
public:
    std::string getStartingLine();
    std::string getHeader(std::string key);
    std::string getMessageBody();
	std::string toString();
};
