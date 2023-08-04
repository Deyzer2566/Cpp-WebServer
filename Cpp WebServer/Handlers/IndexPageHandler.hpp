#pragma once
#include "../HTTPHandler.hpp"
class IndexPageHandler: public HTTPHandler
{
public:
    void executeRequest(HTTPRequest request, HTTPClient& httpClient) override;
};
