#include "../WebSocketHandler.hpp"
class FirstWebSocket: public WebSocketHandler
{
public:
    void onMessage(WebSocketPacket packet, WebSocketClient & client) override;
};
