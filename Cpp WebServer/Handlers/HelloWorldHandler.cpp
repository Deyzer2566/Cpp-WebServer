#include "HelloWorldHandler.hpp"
#include "../Packets.hpp"
void HelloWorldHandler::executeRequest(HTTPRequest request, HTTPClient& httpClient)
{
    if(request.method == "GET" && request.uri=="/")
    {
        HTTPResponse resp;
        resp.code=200;
        resp.message="OK";
        resp.messageBody = "<html><body><p>Hello, world!</p></body></html>";
        resp.headers["Connection"]="close";
        resp.compile_packet();
        httpClient.send(resp);
    }
}
