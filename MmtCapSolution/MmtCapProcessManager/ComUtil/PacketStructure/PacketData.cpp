/*
 * PacketData.cpp
 *
 *  Created on: 2017/07/06
 *      Author: attractor
 */

#include <string.h>

#include "PacketData.h"

PacketData::PacketData() {
	_packet = NULL;
	_packetSize = 0;
}

PacketData::~PacketData() {
	if(_packet != NULL){
		delete [] _packet;
	}
}

int PacketData::SetPacket(const unsigned char* packet, unsigned long packetSize, unsigned long* realPacketSize) {
	if(packet == NULL){
		return -1;
	}
	_packetSize = packetSize;
	_packet = new unsigned char[_packetSize];
	memcpy(_packet, packet, _packetSize);
	if(realPacketSize != NULL){
		*realPacketSize = _packetSize;
	}
	return 0;
}

int PacketData::GetPacket(unsigned char* packetBuffer, unsigned long packetBufferSize, unsigned long* packetSize) {
	if(packetBuffer == NULL){
		return -1;
	}
	if(packetBufferSize < _packetSize){
		return -1;
	}
	memcpy(packetBuffer, _packet, _packetSize);
	if(packetSize != NULL){
		*packetSize = _packetSize;
	}
	return 0;
}

unsigned long PacketData::GetPacketSize() {
	return _packetSize;
}


