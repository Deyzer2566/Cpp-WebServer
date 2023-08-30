#pragma once
#include <string>
#include <map>
#include "HTTPPacket.hpp"
class HTTPRequest: public HTTPPacket
{
private:
	std::string method;
	std::string uri;
	std::string version;
	std::map<std::string, std::string> params;
public:
	HTTPRequest();
	HTTPRequest(std::string _X);
	std::string getMethod();
	std::string getURI();
	std::string getVersion();
	std::map<std::string,std::string> getParams();
};
