#include "IndexPageHandler.hpp"
void IndexPageHandler::executeRequest(HTTPRequest request, HTTPClient& httpClient)
{
    if(request.getMethod() == "GET" && request.getURI()=="/index")
    {
        HTTPResponse resp;
        resp.setCode(200);
        resp.setMessage("OK");
        resp.setMessageBody("<html><body><p>You openned the index page!</p></body></html>");
        resp.setHeader("Connection","close");
        resp.compile_packet();
        httpClient.send(resp);
    }
}
