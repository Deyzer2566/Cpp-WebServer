#pragma once
#include "HTTPClient.hpp"
#include "Packets.hpp"
class HTTPHandler
{
public:
	virtual void executeRequest(HTTPRequest request, HTTPClient& httpClient){}
};
