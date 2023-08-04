#pragma once
#include "../HTTPHandler.hpp"
class HelloWorldHandler : public HTTPHandler
{
public:
    void executeRequest(HTTPRequest request, HTTPClient& httpClient) override;
};
