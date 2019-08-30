
#include "../Util/BinData.h"
#include "MmtpPacket.h"


MmtpPacket::MmtpPacket()
{
	_version = 0;
	_packet_counter_flag = false;
	_FEC_type = 0;
	_extension_flag = false;
	_RAP_flag = false;
	_payload_type = 0;
	_packet_id = 0;
	_timestamp = 0;
	_packet_sequence_number = 0;

	//if(packet_counter_flag)
	_packet_counter = 0;

	//if(extension_flag)
	_extension_type = 0;
	_extension_length = 0;
	_header_extension_bytes = NULL;

	_header_length = MMTP_HEADER_LEN;

	_MMTP_payload_length = 0;
	_MMTP_payload = NULL;
}


MmtpPacket::~MmtpPacket()
{
	if (_header_extension_bytes != NULL) {
		delete[] _header_extension_bytes;
		_header_extension_bytes=NULL;
	}
	if (_MMTP_payload != NULL) {
		delete[] _MMTP_payload;
		_MMTP_payload = NULL;
	}
}

int MmtpPacket::SetPacket(const unsigned char* packet, unsigned long packetSize, unsigned long* realPacketSize) {

	_header_length = 0;

	if (packetSize < (_header_length+ MMTP_HEADER_LEN)) {
		return -1;
	}
	const unsigned char* wp = packet;
	int bitCount = 0;
	int bitPos = 0;

	//version (2bit)
	bitCount = 2;
	_version = BinData::GetBit(bitCount, bitPos, *wp);
	bitPos += bitCount;

	//packet_counter_flag (1bit)
	bitCount = 1;
	_packet_counter_flag = (BinData::GetBit(bitCount, bitPos, *wp) != 0) ? true : false;
	bitPos += bitCount;

	//FEC_type (2bit)
	bitCount = 2;
	_FEC_type = BinData::GetBit(bitCount, bitPos, *wp);
	bitPos += bitCount;

	//reserved (1bit)
	bitPos += 1;

	//extension_flag (1bit)
	bitCount = 1;
	_extension_flag = (BinData::GetBit(bitCount, bitPos, *wp) != 0) ? true : false;
	bitPos += bitCount;

	//RAP_flag (1bit)
	bitCount = 1;
	_RAP_flag = (BinData::GetBit(bitCount, bitPos, *wp) != 0) ? true : false;
	bitPos += bitCount;

	wp++;
	_header_length++;

	//reserved (2bit)
	//payload_type (6bit)
	_payload_type = *wp & 0x3F;

	wp++;
	_header_length++;

	//packet_id (16bit)
	_packet_id = BinData::Get16bit(wp);
	_header_length+=2;

	//timestamp (32bit)
	_timestamp = BinData::Get32bit(wp);
	_header_length += 4;

	//packet_sequence_number
	_packet_sequence_number = BinData::Get32bit(wp);
	_header_length += 4;

	if (_packet_counter_flag) {
		if (packetSize < (_header_length+4)) {
			return -1;
		}
		_packet_counter = BinData::Get32bit(wp);
		_header_length += 4;
	}

	if (_header_extension_bytes != NULL) {
		delete[] _header_extension_bytes;
		_header_extension_bytes = NULL;
	}
	if (_extension_flag) {
		if (packetSize < (_header_length+4)) {
			return -1;
		}
		_extension_type = BinData::Get16bit(wp);
		_extension_length = BinData::Get16bit(wp);
		_header_length += 4;

		if (_extension_length > 0) {
			if (packetSize < (_header_length+_extension_length)) {
				return -1;
			}
			_header_extension_bytes = new unsigned char[_extension_length];
			memset(_header_extension_bytes, NULL, _extension_length);
			memcpy(_header_extension_bytes, wp, _extension_length);
			wp += _extension_length;
			_header_length += _extension_length;
		}
	}

	if (_MMTP_payload != NULL) {
		delete[] _MMTP_payload;
		_MMTP_payload = NULL;
	}

	_MMTP_payload_length = packetSize - _header_length;
	if (_MMTP_payload_length > 0) {
		_MMTP_payload = new unsigned char[_MMTP_payload_length];
		memset(_MMTP_payload, NULL, _MMTP_payload_length);
		memcpy(_MMTP_payload, wp, _MMTP_payload_length);
	}

	if (realPacketSize != NULL) {
		*realPacketSize = _packetSize;
	}

	return 0;
}

int MmtpPacket::GetPacket(unsigned char* packetBuffer, unsigned long packetBufferSize, unsigned long* packetSize) {
	if (packetSize != NULL) {
		*packetSize = 0;
	}
	if (packetBuffer == NULL) {
		return -1;
	}

	if (_packet_counter_flag) {
		_header_length = MMTP_HEADER_LEN + 1 + _extension_length;
	}
	else {
		_header_length = MMTP_HEADER_LEN + 0 + _extension_length;
	}

	if (packetBufferSize < (_header_length + _MMTP_payload_length)) {
		return -1;
	}
	unsigned char* wp = packetBuffer;

	BinData::SetBit(_version, 2, 0, wp);
	BinData::SetBit(_packet_counter_flag, 1, 2, wp);
	BinData::SetBit(_FEC_type, 2, 3, wp);
	BinData::SetBit(0x1, 1, 5, wp);
	BinData::SetBit(_extension_flag, 1, 6, wp);
	BinData::SetBit(_RAP_flag, 1, 7, wp);

	wp++;

	BinData::Set8bit((_payload_type | 0xC0), wp);
	BinData::Set16bit(_packet_id, wp);
	BinData::Set32bit(_timestamp, wp);
	BinData::Set32bit(_packet_sequence_number, wp);

	if (_packet_counter_flag) {
		BinData::Set32bit(_packet_counter, wp);
	}

	if (_extension_flag) {
		BinData::Set16bit(_extension_type, wp);
		BinData::Set16bit(_extension_length, wp);
		if (_extension_length > 0) {
			memcpy(wp, _header_extension_bytes, _extension_length);
			wp += _extension_length;
		}
	}

	unsigned long payloadLen = 0;
	if (_MMTP_payload != NULL) {
		memcpy(wp, _MMTP_payload, _MMTP_payload_length);
	}

	if (packetSize != NULL) {
		*packetSize = (_header_length + _MMTP_payload_length);
	}
	return 0;
}

int MmtpPacket::setHeader_extension_bytes(const unsigned char* header_extension_bytes, unsigned short extension_length) {
	if (header_extension_bytes == NULL) {
		return -1;
	}
	if (_header_extension_bytes != NULL) {
		delete[] _header_extension_bytes;
		_header_extension_bytes = NULL;
	}
	_extension_length = extension_length;
	if (_extension_length > 0) {
		_header_extension_bytes = new unsigned char[_extension_length];
		memcpy(_header_extension_bytes, header_extension_bytes, _extension_length);
	}
	return 0;
}

unsigned long MmtpPacket::getHeaderLength() {
	if (_packet_counter_flag) {
		_header_length = MMTP_HEADER_LEN + 1 + _extension_length;
	}
	else {
		_header_length = MMTP_HEADER_LEN + 0 + _extension_length;
	}
	return _header_length;
}

int MmtpPacket::setMMTP_payload(const unsigned char* MMTP_payload, unsigned long MMTP_payload_length) {
	if (MMTP_payload == NULL) {
		return -1;
	}
	if (_MMTP_payload != NULL) {
		delete[] _MMTP_payload;
		_MMTP_payload = NULL;
	}
	_MMTP_payload_length = MMTP_payload_length;
	if (_MMTP_payload_length > 0) {
		_MMTP_payload = new unsigned char[_MMTP_payload_length];
		memcpy(_MMTP_payload, MMTP_payload, _MMTP_payload_length);
	}
	return 0;
}
