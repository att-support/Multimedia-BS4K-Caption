/*
 * PacketData.h
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#ifndef PACKETSTRUCTURE_PACKETDATA_H_
#define PACKETSTRUCTURE_PACKETDATA_H_

#include <stddef.h>

class PacketData {
public:
	PacketData();
	virtual ~PacketData();
	virtual int SetPacket(const unsigned char* packet, unsigned long packetSize, unsigned long* realPacketSize);
	virtual int GetPacket(unsigned char* packetBuffer, unsigned long packetBufferSize, unsigned long* packetSize);
	virtual unsigned long GetPacketSize();

protected:
	unsigned char* _packet;
	unsigned long _packetSize;
};

#endif /* PACKETSTRUCTURE_PACKETDATA_H_ */
