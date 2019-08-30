/*
 * PacketType.h
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#ifndef PACKETSTRUCTURE_PACKETTYPE_H_
#define PACKETSTRUCTURE_PACKETTYPE_H_

typedef unsigned short PACKET_TYPE;

class PacketType {
public:
	static const PACKET_TYPE Type_IPv4 = 0x0800;
	static const PACKET_TYPE Type_ARP = 0x0806;
	static const PACKET_TYPE Type_AppleTalk = 0x809b;
	static const PACKET_TYPE Type_IEEE802_1q = 0x8100;
	static const PACKET_TYPE Type_IPX = 0x8137;
	static const PACKET_TYPE Type_IPv6 = 0x86dd;
	static const PACKET_TYPE Type_Unknown = 0x0000;

private:
	PacketType() {};
	virtual ~PacketType() {};
};

#endif /* PACKETSTRUCTURE_PACKETTYPE_H_ */
