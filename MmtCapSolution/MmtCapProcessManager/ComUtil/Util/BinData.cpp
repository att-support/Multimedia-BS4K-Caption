
#include "BinData.h"


BinData::BinData()
{
}


BinData::~BinData()
{
}

unsigned char BinData::GetUpper4bit(const unsigned char*& srcData)
{
	unsigned char val = GetBit(4, 0, *srcData);
	return val;
}

unsigned char BinData::GetLower4bit(const unsigned char*& srcData)
{
	unsigned char val = GetBit(4, 4, *srcData);
	srcData++;
	return val;
}

unsigned char BinData::Get8bit(const unsigned char*& srcData)
{
	unsigned char value = *srcData;
	srcData++;
	return value;
}

unsigned short BinData::Get16bit(const unsigned char*& srcData)
{
	unsigned short value = 0;
	value |= (*srcData << 8) & 0xFF00;
	srcData++;
	value |= *srcData;
	srcData++;
	return value;
}

unsigned long BinData::Get24bit(const unsigned char*& srcData)
{
	unsigned long value = 0;
	value |= (*srcData << 16) & 0xFF0000;
	srcData++;
	value |= (*srcData <<  8) & 0x00FF00;
	srcData++;
	value |= *srcData;
	srcData++;
	return value;
}

unsigned long BinData::Get32bit(const unsigned char*& srcData)
{
	unsigned long value = 0;
	value |= (*srcData << 24) & 0xFF000000;
	srcData++;
	value |= (*srcData << 16) & 0x00FF0000;
	srcData++;
	value |= (*srcData <<  8) & 0x0000FF00;
	srcData++;
	value |= *srcData;
	srcData++;
	return value;
}

unsigned long long BinData::Get64bit(const unsigned char*& srcData)
{
	uint64_t value = 0;
	value |= (static_cast<uint64_t>(*srcData) << 56) & 0xFF00000000000000ULL;
	srcData++;
	value |= (static_cast<uint64_t>(*srcData) << 48) & 0x00FF000000000000ULL;
	srcData++;
	value |= (static_cast<uint64_t>(*srcData) << 40) & 0x0000FF0000000000ULL;
	srcData++;
	value |= (static_cast<uint64_t>(*srcData) << 32) & 0x000000FF00000000ULL;
	srcData++;
	value |= (static_cast<uint64_t>(*srcData) << 24) & 0x00000000FF000000ULL;
	srcData++;
	value |= (static_cast<uint64_t>(*srcData) << 16) & 0x0000000000FF0000ULL;
	srcData++;
	value |= (static_cast<uint64_t>(*srcData) <<  8) & 0x000000000000FF00ULL;
	srcData++;
	value |= static_cast<uint64_t>(*srcData);
	srcData++;
	return value;
}

unsigned char BinData::GetBit(int bitCount, int startBitFromMSB, unsigned char srcData)
{
	if ((bitCount == 0) || (bitCount > 8)){
		return 0;
	}
	if (startBitFromMSB >= 8){
		return 0;
	}
	if ((startBitFromMSB + bitCount) > 8){
		return 0;
	}

	unsigned char mask = 0;
	for (int i = 0; i < bitCount; i++){
		mask |= (1 << i);
	}
	unsigned char value = (srcData >> (8 - (startBitFromMSB + bitCount))) & mask;
	return value;
}

void BinData::SetUpper4bit(unsigned char data, unsigned char*& dstData)
{
	*dstData = (unsigned char)(((data << 4) & 0xF0) | (*dstData & 0x0F));
}
void BinData::SetLower4bit(unsigned char data, unsigned char*& dstData)
{
	*dstData = (unsigned char)((*dstData & 0xF0) | (data & 0x0F));
	dstData++;
}
void BinData::Set8bit(unsigned char data, unsigned char*& dstData)
{
	*dstData = (unsigned char)(data & 0xFF);
	dstData++;
}
void BinData::Set16bit(unsigned short data, unsigned char*& dstData)
{
	*dstData = (unsigned char)((data >> 8) & 0xFF);
	dstData++;
	*dstData = (unsigned char)(data  & 0xFF);
	dstData++;
}
void BinData::Set24bit(unsigned long data, unsigned char*& dstData)
{
	*dstData = (unsigned char)((data >> 16) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 8) & 0xFF);
	dstData++;
	*dstData = (unsigned char)(data & 0xFF);
	dstData++;
}
void BinData::Set32bit(unsigned long data, unsigned char*& dstData)
{
	*dstData = (unsigned char)((data >> 24) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 16) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 8) & 0xFF);
	dstData++;
	*dstData = (unsigned char)(data & 0xFF);
	dstData++;
}
void BinData::Set64bit(unsigned long long data, unsigned char*& dstData)
{
	*dstData = (unsigned char)((data >> 56) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 48) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 40) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 32) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 24) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 16) & 0xFF);
	dstData++;
	*dstData = (unsigned char)((data >> 8) & 0xFF);
	dstData++;
	*dstData = (unsigned char)(data & 0xFF);
	dstData++;
}

void BinData::SetBit(unsigned char data, int bitCount, int startBitFromMSB, unsigned char*& dstData)
{
	if ((bitCount == 0) || (bitCount > 8)){
		return;
	}
	if (startBitFromMSB >= 8){
		return;
	}
	if ((startBitFromMSB + bitCount) > 8){
		return;
	}

	unsigned char setData = (unsigned char)(data << (8 - (startBitFromMSB + bitCount)));
	int mask = 0;
	for (int i = 0; i < bitCount; i++){
		mask |= (1 << i);
	}
	mask <<= (8 - (startBitFromMSB + bitCount));
	*dstData &= ~mask;
	*dstData |= (setData & mask);
}

void BinData::SumOnesComplement(unsigned short& sumVal, unsigned long addVal)
{
	unsigned long tmpSum = sumVal;
	tmpSum += addVal;
	while(tmpSum > 0xFFFF){
		tmpSum = (tmpSum & 0xFFFF) + (tmpSum >> 16);
	}
	sumVal = static_cast<unsigned short>(tmpSum);
}

void BinData::ConvOnesComplement(unsigned short& data) {
	data = ~data;
}
