#include "FirstWebSocket.hpp"
#include <iostream>
void FirstWebSocket::onMessage(WebSocketPacket packet, WebSocketClient & client)
{
    std::cout<<packet.data<<std::endl;
    for(int i = 0;i<4;i++)
        packet.Mask[i]=0;
    packet.isMask=false;
    client.send(packet.toArray());
}
