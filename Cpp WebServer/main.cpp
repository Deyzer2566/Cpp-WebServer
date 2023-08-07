#include <iostream>
#include "Server.hpp"
#include <chrono>
#include <thread>
#include "Handlers/HelloWorldHandler.hpp"
#include "Handlers/IndexPageHandler.hpp"
#include "Handlers/FirstWebSocket.hpp"
int main(int argc, char **argv)
{
#ifdef _WIN32
	WSAData ws;
	WSAStartup(0x0202, &ws);
#endif
	int port = 80;
	try
	{
        Server serv(port);
        serv.addHTTPHandler(new HelloWorldHandler());
        serv.addHTTPHandler(new IndexPageHandler());
        serv.addWebSocketHandler("/first",new FirstWebSocket());
        while (true)
        {
            serv.cycle();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
	}
	catch (std::string error)
	{
	    std::cout<<error<<std::endl;
	}
	return 0;
}
