#pragma once
#include "WebSocketClient.hpp"
#include "WebSocketPacket.hpp"
class WebSocketHandler
{
public:
    virtual void onMessage(WebSocketPacket webSocketPacket, WebSocketClient& webSocketClient){}
    virtual void onConnect(WebSocketClient& webSocketClient){}
    virtual void onDisconnect(WebSocketClient& webSocketClient){}
};
