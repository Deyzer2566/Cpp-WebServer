#pragma once
#include "TCPClient.hpp"
#include "HTTPResponse.hpp"
class HTTPClient :public TCPClient
{
public:
	HTTPClient(TCPClient client);
	int send(HTTPResponse& resp);
};
