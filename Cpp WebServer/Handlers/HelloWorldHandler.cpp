#include "HelloWorldHandler.hpp"
#include "../HTTPRequest.hpp"
void HelloWorldHandler::executeRequest(HTTPRequest request, HTTPClient& httpClient)
{
    if(request.getMethod() == "GET" && request.getURI()=="/")
    {
        HTTPResponse resp;
        resp.setCode(200);
        resp.setMessage("OK");
        resp.setMessageBody("<html><body><p>Hello, world!</p></body></html>");
        resp.setHeader("Connection","close");
        resp.compile_packet();
        httpClient.send(resp);
    }
}
