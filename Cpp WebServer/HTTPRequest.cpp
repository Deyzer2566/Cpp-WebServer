#include "HTTPRequest.hpp"
#include "Utilite.hpp"
#include <cstdint>
HTTPRequest::HTTPRequest(){}
HTTPRequest::HTTPRequest(std::string _X)
{
	std::vector<std::string> strings = split(_X, "\r\n");
	if (strings.size() < 2)
	    throw std::string("Can't split the packet");
	std::vector<std::string> startingLine = split(strings[0], " ");
	if (startingLine.size() != 3)
	    throw std::string("Invalid starting line format");
	method = startingLine[0];
	uri = startingLine[1];
	if (uri.find("%") != uri.npos)
	{
		Unicode str = UTF_8_to_Unicode(uri, "%");
		uri.clear();
		uint16_t AB = 1040;
		uint16_t YAB = 1071;
		uint16_t A = 1072;
		uint16_t YA = 1103;
		for (uint16_t s : str){
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
			    throw std::string("Can't split the params");
		}
	}
	version = startingLine[2];
	size_t i = 1;
	for (; i<strings.size() && strings[i] != ""; i++)
	{
		size_t pos = strings[i].find(": ");
		if (pos == strings[i].npos)
		{
		    throw std::string("Invalid header format: "+strings[i]);
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
}

std::string HTTPRequest::getMethod()
{
    return method;
}
std::string HTTPRequest::getURI()
{
    return uri;
}
std::string HTTPRequest::getVersion()
{
    return version;
}
std::map<std::string,std::string> HTTPRequest::getParams()
{
    return params;
}
