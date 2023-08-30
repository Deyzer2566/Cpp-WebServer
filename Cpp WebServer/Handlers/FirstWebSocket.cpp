#include "FirstWebSocket.hpp"
#include <iostream>
void FirstWebSocket::onMessage(WebSocketPacket packet, WebSocketClient & client)
{
    std::cout<<packet.getData()<<std::endl;
    WebSocketPacket ans(WebSocketPacket::TEXT,false,packet.getData());
    client.send(ans.toString());
}
