#include "IndexPageHandler.hpp"
void IndexPageHandler::executeRequest(HTTPRequest request, HTTPClient& httpClient)
{
    if(request.method == "GET" && request.uri=="/index")
    {
        HTTPResponse resp;
        resp.code=200;
        resp.message="OK";
        resp.messageBody = "<html><body><p>You openned the index page!</p></body></html>";
        resp.headers["Connection"]="close";
        resp.compile_packet();
        httpClient.send(resp);
    }
}
