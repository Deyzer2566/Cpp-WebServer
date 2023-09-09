#include "WebSocketPacket.hpp"
WebSocketPacket::WebSocketPacket(Opcode opcode, bool useMask, std::string data)
{
	this->opcode = opcode;
	fin = 1;
	RSV[0] = 0; RSV[1] = 0; RSV[2] = 0;
	hasMask = useMask;
	mask[0] = mask[1] = mask[2] = mask[3] = 0;
	if (hasMask)
		for (int i = 0; i < 4; i++)
			mask[i] = rand() % 256;
	length = data.size();
	this->data = data;
}
std::string WebSocketPacket::toString()
{
	std::string ret;
	ret.push_back((fin & 1) << 7 | (RSV[0] & 1) << 6 | (RSV[1] & 1) << 5 | (RSV[2] & 1) << 4 | (opcode & 0x0F));
	ret.push_back((hasMask & 1) << 7);
	if (length < 126)
		ret.back() |= length & 0x7F;
	else
		if (length <= 0xFFFF)
		{
			ret.back() |= 126;
			for (int i = 1; i >= 0; i--)
				ret.push_back(length&(0xFF << (8 * i)));
		}
		else
		{
			ret.back() |= 127;
			for (int i = 3; i >= 0; i--)
				ret.push_back(length&(0xFF << (8 * i)));
		}
	for (int i = 0; i < 4 && hasMask; i++) {
		ret.push_back(mask[i]);
	}
	for (size_t i = 0; i < length; i++)
	{
		ret += data[i] ^ mask[i % 4];
	}
	return ret;
}
WebSocketPacket::WebSocketPacket(std::string message)
{
    int offset = 0;
	fin = ((message[offset] & (0x80)) >> 7) & 0x01;
	RSV[0] = ((message[offset] & (0x40)) >> 6) & 0x01;
	RSV[1] = ((message[offset] & (0x20)) >> 5) & 0x01;
	RSV[2] = ((message[offset] & (0x10)) >> 4) & 0x01;
	switch(message[offset] & 0x0F)
	{
    case 0:
        opcode=CONT;
        break;
    case 1:
        opcode=TEXT;
        break;
    case 2:
        opcode=BIN;
        break;
    case 8:
        opcode=CLOSE;
        break;
    case 9:
        opcode=PING;
        break;
    case 0xA:
        opcode=PONG;
        break;
	}
	offset++;
	hasMask = (message[offset] & (0x80)) >> 7;
	length = message[offset] & (0x7F);
	offset++;
	if (length == 126)
	{
		length = ((message[offset] & 0xFF) << 8) | (message[offset + 1] & 0xFF);
		offset += 2;
	}
	else if (length == 127)
	{
		length = ((uint64_t)message[offset] << 56) | ((uint64_t)message[offset + 1] << 48) | ((uint64_t)message[offset + 2] << 40) | ((uint64_t)message[offset + 3] << 32) | ((uint64_t)message[offset + 4] << 24) | ((uint64_t)message[offset + 5] << 16) | ((uint64_t)message[offset + 6] << 8) | (uint64_t)message[offset + 7];
		offset += 8;
	}
	if (hasMask)
	{
		mask[0] = message[offset];
		mask[1] = message[offset + 1];
		mask[2] = message[offset + 2];
		mask[3] = message[offset + 3];
		offset += 4;
	}
	data.resize(length);
	for (size_t i = 0; i < length; i++)
		data[i] = (message[i + offset]) ^ mask[i % 4];
}
WebSocketPacket::Opcode WebSocketPacket::getOpcode()
{
    return opcode;
}
std::string WebSocketPacket::getData()
{
    return data;
}
void WebSocketPacket::setMask(std::array<uint8_t, 4> mask)
{
    this->mask = mask;
}
