/*
 * Conv.cpp
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#include <string.h>
#include <errno.h>
#include <math.h>	/* HUGE_VALを使用するために必要 */
#include <sys/stat.h>
#include <stdio.h>
#include <climits>

#include "StringUtil.h"
#include "Conv.h"

Conv::Conv() {
}

Conv::~Conv() {
}

int Conv::DecStrToInt(const std::string& str) {
	return atoi(str.c_str());
}

int Conv::DecStrToInt(const std::string& str, int& retValue) {
	retValue = 0;

	char *endptr;
	errno = 0;
	long value = strtol(str.c_str(), &endptr, 10);
	if(endptr == str.c_str()){
		return -1;
	}
	else if('\0' != *endptr){
		//変換不可
		return -1;
	}
	//TODO: LONG_MIN、LONG_MAX、INT_MAX、INT_MIN がなぜか未定義
//	else if ((LONG_MIN == value || LONG_MAX == value) && ERANGE == errno) {
//		//範囲外
//		return -1;
//	}
//	else if (value > INT_MAX){
//		//範囲外
//		return -1;
//	}
//	else if (value < INT_MIN){
//		//範囲外
//		return -1;
//	}
	else{
		//OK
	}

	retValue = (int)value;
	return 0;
}

int Conv::HexStrToUL(const std::string& str, unsigned long& retValue)
{
	retValue = 0;

	char *endptr;
	errno = 0;
	long long int value = strtoll(str.c_str(), &endptr, 16);
	if(endptr == str.c_str()){
		return -1;
	}
	else if('\0' != *endptr){
		//変換不可
		return -1;
	}
	else if ((LLONG_MIN == value || LLONG_MAX == value) && ERANGE == errno) {
		//範囲外
		return -1;
	}
	else{
		//OK
	}

	retValue = (unsigned long)value;
	return 0;

}

std::string Conv::IntToDecStr(int value) {
	char buf[32];
	sprintf(buf,"%d",value);
	return std::string(buf);
}

std::string Conv::IntToDecStr(int value, int digit) {
	char buf[32];
	sprintf(buf,"%d",value);
	std::string decStr(buf);
	size_t strLen = decStr.size();
	if(strLen < (size_t)digit){
		decStr = std::string((digit - strLen),'0') + decStr;
	}
	else if (strLen > (size_t)digit){
		decStr = decStr.substr((strLen - (size_t)digit));
	}
	return decStr;
}

std::string Conv::ULToDecStr(unsigned long value)
{
	char buf[32];
	sprintf(buf,"%lu",value);
	return std::string(buf);
}

std::string Conv::SysClockToDecStr(SYSCLK value)
{
	char buf[64];
	sprintf(buf,"%llu",value);
	return std::string(buf);
}

std::string Conv::ULtoHexStr(unsigned long value, int digit, int flag)
{
	char buf[32];
	sprintf(buf,"%lx",value);
	std::string hexStr(buf);
	size_t strLen = hexStr.size();
	if(strLen < (size_t)digit){
		hexStr = std::string((digit - strLen),'0') + hexStr;
	}
	else if (strLen > (size_t)digit){
		hexStr = hexStr.substr((strLen - (size_t)digit));
	}
	if (flag == 0) {
		hexStr = "0x" + hexStr;
	}
	return hexStr;
}

std::string Conv::ByteToHexStr(char value) {
	char buf[8];
	sprintf(buf,"%02X",(unsigned char)value);
	std::string hexStr(buf);
	return hexStr;
}

unsigned char Conv::ByteToBcd2(unsigned char value) {
	if(value > 99){
		return 0x00;
	}
	unsigned char bcd;
	bcd = ((unsigned char)(value / 10) << 4) & 0xF0;
	bcd |= (value % 10) & 0x0F;
	return bcd;
}

unsigned short Conv::ULToBcd4(unsigned long value) {
	if(value > 9999){
		return 0;
	}
	unsigned long bcd;
	unsigned long mod;
	bcd = ((unsigned short)(value / 1000) << (4*3)) & 0xF000;
	mod = value % 1000;
	bcd |= ((unsigned short)(mod / 100) << (4*2)) & 0x0F00;
	mod = mod % 100;
	bcd |= ((unsigned short)(mod / 10) << 4) & 0x00F0;
	mod = mod % 10;
	bcd |= mod & 0x000F;
	return (unsigned short)bcd;
}

int Conv::Bcd4ToInt(unsigned short bcd) {
	int value;
	value = (int)(((bcd >> 12) & 0xF) * 1000);
	value += (int)(((bcd >> 8) & 0xF) * 100);
	value += (int)(((bcd >> 4) & 0xF) * 10);
	value += (int)(bcd & 0xF);
	return value;
}

int Conv::Bcd2ToInt(unsigned char bcd) {
	int value;
	value = (int)(((bcd >> 4) & 0xF) * 10);
	value += (int)(bcd & 0xF);
	return value;
}

bool Conv::BoolStrToBool(const std::string& str) {
	if((str == "TRUE") || (str == "true") || (str == "True") || (str == "1") ||
		(str == "YES") || (str == "yes") || (str == "Yes")){
		return true;
	}
	else{
		return false;
	}
}

//小数値文字列→double型数値変換
double Conv::FStrToDouble(const std::string& str) {
	double ret;
	FStrToDouble(str, ret);
	return ret;
}

int Conv::FStrToDouble(const std::string& str, double& ret) {
	char *endptr;
	ret = strtod(str.c_str(), &endptr);
	if (errno != ERANGE) {
		if (*endptr != '\0'){
			ret = 0.0;
			return -1;
		}
	}
	else if (ret == HUGE_VAL){
		ret = 0.0;
		return -1;
	}

	return 0;
}

