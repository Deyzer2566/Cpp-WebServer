#include "WebSocketClient.hpp"
#include <string>
#include <thread>
#include "Packets.hpp"
WebSocketClient::WebSocketClient(int socket, sockaddr_in addr) :TCPClient(socket, addr){}

WebSocketClient::WebSocketClient(TCPSocket& sock) : TCPClient(sock){}

WebSocketClient::WebSocketClient(){}
