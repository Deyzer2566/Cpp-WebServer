#include "HTTPPacket.hpp"
std::string HTTPPacket::getHeader(std::string key)
{
    return headers.at(key);
}
std::string HTTPPacket::getMessageBody()
{
    return messageBody;
}
std::string HTTPPacket::getStartingLine()
{
    return startingLine;
}
std::string HTTPPacket::toString()
{
	std::string ret;
	ret += startingLine + "\n";
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		ret += it->first + ": " + it->second+"\n";
	ret += "\n";
	ret.insert(ret.end(),messageBody.begin(),messageBody.end());
	return ret;
}
