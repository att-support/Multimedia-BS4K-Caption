/*
 * StringUtil.h
 *
 *  Created on: 2017/07/04
 *      Author: attractor
 */

#ifndef UTIL_STRINGUTIL_H_
#define UTIL_STRINGUTIL_H_

#include <string>
#include <vector>

class StringUtil {
public:
	StringUtil();
	virtual ~StringUtil();

	static std::string Trim(const std::string& srcStr, const std::string& trimChars);
	static void Split(const std::string& srcStr, const std::string& delimiters, std::vector<std::string>& dstStrList);
	static std::string Format(const std::string fmt, ...);
	static std::string Replace(const std::string& srcStr, const std::string& findStr, const std::string& replaceStr);
};

#endif /* UTIL_STRINGUTIL_H_ */
