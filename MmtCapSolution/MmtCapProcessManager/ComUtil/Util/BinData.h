/*
 * BinData.h
 *
 *  Created on: 2017/07/02
 *      Author: attractor
 */

#ifndef UTIL_BINDATA_H_
#define UTIL_BINDATA_H_
#include <stdint.h>
class BinData
{
public:
	BinData();
	virtual ~BinData();

	static unsigned char GetUpper4bit(const unsigned char*& srcData);
	static unsigned char GetLower4bit(const unsigned char*& srcData);
	static unsigned char Get8bit(const unsigned char*& srcData);
	static unsigned short Get16bit(const unsigned char*& srcData);
	static unsigned long Get24bit(const unsigned char*& srcData);
	static unsigned long Get32bit(const unsigned char*& srcData);
	static unsigned long long Get64bit(const unsigned char*& srcData);

	static void SetUpper4bit(unsigned char data, unsigned char*& dstData);
	static void SetLower4bit(unsigned char data, unsigned char*& dstData);
	static void Set8bit(unsigned char data, unsigned char*& dstData);
	static void Set16bit(unsigned short data, unsigned char*& dstData);
	static void Set24bit(unsigned long data, unsigned char*& dstData);
	static void Set32bit(unsigned long data, unsigned char*& dstData);
	static void Set64bit(unsigned long long data, unsigned char*& dstData);

	static unsigned char GetBit(int bitCount, int startBitFromMSB, unsigned char srcData);
	static void SetBit(unsigned char setData, int bitCount, int startBitFromMSB, unsigned char*& dstData);

	static void SumOnesComplement(unsigned short& sumVal, unsigned long addVal);
	static void ConvOnesComplement(unsigned short& data);

};

#endif /* UTIL_BINDATA_H_ */
