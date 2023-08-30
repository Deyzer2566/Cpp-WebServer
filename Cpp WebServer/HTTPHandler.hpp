#pragma once
#include "HTTPClient.hpp"
#include "HTTPRequest.hpp"
class HTTPHandler
{
public:
	virtual void executeRequest(HTTPRequest request, HTTPClient& httpClient){}
};
