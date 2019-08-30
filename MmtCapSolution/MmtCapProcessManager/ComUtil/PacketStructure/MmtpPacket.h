/*
* MmtpPacket.h
*
*  Created on: 2017/07/06
*      Author: attractor
*/

#ifndef PACKETSTRUCTURE_MMTPPACKET_H_
#define PACKETSTRUCTURE_MMTPPACKET_H_

#include <string.h>
#include "PacketData.h"

#define MMTP_HEADER_LEN 12

class MmtpPacket :
	public PacketData
{
public:
	MmtpPacket();
	virtual ~MmtpPacket();
	int SetPacket(const unsigned char* packet, unsigned long packetSize, unsigned long* realPacketSize);
	int GetPacket(unsigned char* packetBuffer, unsigned long packetBufferSize, unsigned long* packetSize);

	//
	//getter, setter
	//
	unsigned char getVersion() const {
		return _version;
	}
	void setVersion(unsigned char version) {
		_version = version;
	}

	bool getPacket_counter_flag() const {
		return _packet_counter_flag;
	}
	void setPacket_counter_flag(bool packet_counter_flag) {
		_packet_counter_flag = packet_counter_flag;
	}

	unsigned char getFEC_type() const {
		return _FEC_type;
	}
	void setFEC_type(unsigned char FEC_type) {
		_FEC_type = FEC_type;
	}

	bool getExtension_flag() const {
		return _extension_flag;
	}
	void setExtension_flag(bool extension_flag) {
		_extension_flag = extension_flag;
	}

	bool getRAP_flag() const {
		return _RAP_flag;
	}
	void setRAP_flag(bool RAP_flag) {
		_RAP_flag = RAP_flag;
	}

	unsigned char getPayload_type() const {
		return _payload_type;
	}
	void setPayload_type(unsigned char payload_type) {
		_payload_type = payload_type;
	}

	unsigned short getPacket_id() const {
		return _packet_id;
	}
	void setPacket_id(unsigned short packet_id) {
		_packet_id = packet_id;
	}

	unsigned long getTimestamp() const {
		return _timestamp;
	}
	void setTimestamp(unsigned long timestamp) {
		_timestamp = timestamp;
	}

	unsigned long getPacket_sequence_number() const{
		return _packet_sequence_number;
	}
	void set_packet_sequence_number(unsigned long packet_sequence_number) {
		_packet_sequence_number = packet_sequence_number;
	}

	//if(packet_counter_flag)
	unsigned long getPacket_counter() const {
		return _packet_counter;
	}
	void setPacket_counter(unsigned long packet_counter) {
		_packet_counter = packet_counter;
	}

	//if(extension_flag)
	unsigned short getExtension_type() const{
		return _extension_type;
	}
	void setExtension_type(unsigned short extension_type) {
		_extension_type = extension_type;
	}

	unsigned short getExtension_length() const {
		return _extension_length;
	}

	const unsigned char* getHeader_extension_bytes() const {
		return _header_extension_bytes;
	}

	int setHeader_extension_bytes(const unsigned char* header_extension_bytes, unsigned short extension_length);

	unsigned long getHeaderLength();

	int setMMTP_payload(const unsigned char* MMTP_payload, unsigned long MMTP_payload_length);

	unsigned long getMMTP_payload_length() const {
		return _MMTP_payload_length;
	}

	unsigned char* getMMTP_payload() const {
		return _MMTP_payload;
	}


private:
	unsigned char _version;
	bool _packet_counter_flag;
	unsigned char _FEC_type;
	bool _extension_flag;
	bool _RAP_flag;
	unsigned char _payload_type;
	unsigned short _packet_id;
	unsigned long _timestamp;
	unsigned long _packet_sequence_number;

	//if(packet_counter_flag)
	unsigned long _packet_counter;

	//if(extension_flag)
	unsigned short _extension_type;
	unsigned short _extension_length;
	unsigned char* _header_extension_bytes;

	unsigned long _header_length;

	unsigned long _MMTP_payload_length;
	unsigned char* _MMTP_payload;
};

#endif /* PACKETSTRUCTURE_MMTPPACKET_H_ */


