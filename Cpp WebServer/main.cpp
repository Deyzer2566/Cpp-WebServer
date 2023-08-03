#include <iostream>
#include "Server.hpp"
#include <chrono>
#include <thread>
int main(int argc, char **argv)
{
#ifdef _WIN32
	WSAData ws;
	WSAStartup(0x0202, &ws);
#endif
	int port = 80;
	Server serv("site", port);
	while (true)
	{
		serv.cycle();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
}
