/*
 * Conv.h
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#ifndef UTIL_CONV_H_
#define UTIL_CONV_H_

#include <stdlib.h>
#include <string>
#include "SysClock.h"

class Conv {
public:
	Conv();
	virtual ~Conv();

	static int DecStrToInt(const std::string& str);								//10進数数値文字列→数値変換（チェックなし 戻り値で変換後の値を返す）
	static int DecStrToInt(const std::string& str, int& retValue);				//10進数数値文字列→数値変換 (チェックあり 引数retValueで変換後の値を返す)
	static int HexStrToUL(const std::string& str, unsigned long& retValue);	//16進数数値文字列→数値変換 (チェックあり 引数retValueで変換後の値を返す)
	static std::string IntToDecStr(int value);									//数値→10進数数値文字列
	static std::string IntToDecStr(int value, int digit);						//数値→10進数数値文字列（桁数指定）
	static std::string ULToDecStr(unsigned long value);							//数値→10進数数値文字列
	static std::string SysClockToDecStr(SYSCLK value);							//システムクロック値→10進数数値文字列
	static std::string ULtoHexStr(unsigned long value, int digit, int flag=0);				//数値→16進数数値文字列（桁数指定）
	static std::string ByteToHexStr(char value);									//数値→16進数数値文字列（1byte専用）
	static std::string Utf8ToSjis(const std::string& utf8Str);					//UTF8→SJIS変換
	static unsigned char ByteToBcd2(unsigned char value);						//数値→BCD変換（1byte、00～99 ※超過する場合は00を返す）
	static unsigned short ULToBcd4(unsigned long value);											//数値→BCD変換（4桁）
	static int Bcd4ToInt(unsigned short bcd);												//BCD（4桁）→数値変換
	static int Bcd2ToInt(unsigned char bcd);										//BCD（2桁）→数値変換
	static bool BoolStrToBool(const std::string& str);							//真偽値文字列→bool型変換
	static double FStrToDouble(const std::string& str);							//小数値文字列→double型数値変換
	static int FStrToDouble(const std::string& str, double& ret);				//小数値文字列→double型数値変換
};

#endif /* UTIL_CONV_H_ */
