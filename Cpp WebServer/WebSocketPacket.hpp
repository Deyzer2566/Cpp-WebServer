#pragma once
#include <string>
#include <array>
class WebSocketPacket
{
public:
    enum Opcode{CONT=0,TEXT,BIN,CLOSE=8,PING,PONG};
protected:
	bool fin;
	std::array<bool, 3> RSV;
	bool hasMask;
	std::array<unsigned char, 4> mask;
	std::string data;
	unsigned long long length;
	Opcode opcode;
public:
    Opcode getOpcode();
    std::string getData();
    void setData(std::string data);
    void setMask(std::array<unsigned char, 4> mask);
	std::string toString();
	WebSocketPacket(Opcode opcode,bool useMask, std::string message);
	WebSocketPacket(std::string message);
};
