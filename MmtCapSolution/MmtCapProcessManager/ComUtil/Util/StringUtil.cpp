/*
 * StringUtil.cpp
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include "StringUtil.h"

StringUtil::StringUtil() {
}

StringUtil::~StringUtil() {
}

std::string StringUtil::Trim(const std::string& srcStr, const std::string& trimChars) {
	std::string dstStr = "";
	// Search a not trimChars position from left.
	std::string::size_type left = srcStr.find_first_not_of(trimChars);
	std::string::size_type startPos;
	if (left != std::string::npos)	{
		startPos = left;
	}
	else{
		startPos = 0;
	}
	// Search a not trimChars position from right.
	std::string::size_type right = srcStr.find_last_not_of(trimChars);
	std::string::size_type chars;
	if (right != std::string::npos)	{
		chars = right - startPos + 1;
	}
	else{
		chars = srcStr.size() - startPos;
	}
	// Trimming
	dstStr = srcStr.substr(startPos, chars);
	return dstStr;
}

void StringUtil::Split(const std::string& srcStr, const std::string& delimiters,
		std::vector<std::string>& dstStrList) {

	dstStrList.clear();
	std::string tmpStr = srcStr;
	std::string::size_type pos;
	while((pos = tmpStr.find_first_of(delimiters)) != std::string::npos){
		std::string field = tmpStr.substr(0, pos);
		field = Trim(field," \t\v\r\n");
		dstStrList.push_back(field);
		tmpStr = tmpStr.substr(pos+1,tmpStr.length() - (pos+1));
	}
	tmpStr = Trim(tmpStr," \t\v\r\n");
	if(tmpStr.length() > 0){
		dstStrList.push_back(tmpStr);
	}
}

std::string StringUtil::Format(const std::string fmt, ...) {
	char buf[4096] = {0}; //4KBまで
	va_list va;
	va_start(va, fmt);
	std::vsprintf(buf,fmt.c_str(),va);
	va_end(va);
	return std::string(buf);
}

std::string StringUtil::Replace(const std::string& srcStr, const std::string& findStr, const std::string& replaceStr)
{
	std::string dstStr = srcStr;
	std::string::size_type pos(srcStr.find(findStr));
	while(pos != std::string::npos) {
		dstStr.replace(pos, findStr.size(), replaceStr);
		pos = dstStr.find(findStr, pos + replaceStr.size());
	}
	return dstStr;
}
/*
std::string Replace(std::string String1, std::string String2, std::string String3)
{
	std::string::size_type  Pos(String1.find(String2));

	while (Pos != std::string::npos)
	{
		String1.replace(Pos, String2.length(), String3);
		Pos = String1.find(String2, Pos + String3.length());
	}

	return String1;
}
*/