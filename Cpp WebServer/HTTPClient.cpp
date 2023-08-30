#include "HTTPClient.hpp"
#include "HTTPResponse.hpp"

HTTPClient::HTTPClient(TCPClient client): TCPClient(client){}

int HTTPClient::send(HTTPResponse & resp)
{
	return this->TCPClient::send(resp.toString());
}
